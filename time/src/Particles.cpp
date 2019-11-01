/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "Particles.hpp"

extern "C"
{
#include <logger_reader.h>
}

Particles::Particles(const char* filename)
{
	reader = new logger_reader;

	logger_reader_init(reader, const_cast<char*>(filename), 0);

	timeBegin_ = logger_reader_get_time_begin(reader) + 1e-4;
	timeEnd_   = logger_reader_get_time_end(reader) - 1e-4;

	logger_reader_set_time(reader, timeBegin);

	/* Get the number of particles */
	int n_type      = 0;
	long long n_tot = 0;
	const uint64_t* n_parts
	    = logger_reader_get_number_particles(reader, &n_type);
	for(int i = 0; i < n_type; i++)
	{
		n_tot += n_parts[i];
	}

	particles = new logger_particle[n_tot];

	logger_reader_read_from_index(reader, timeBegin, logger_reader_const,
	                              particles, n_tot);

	double min[3] = {DBL_MAX, DBL_MAX, DBL_MAX},
	       max[3] = {DBL_MIN, DBL_MIN, DBL_MIN};
	double tmin = DBL_MAX, tmax = DBL_MIN;

	for(long long i(0); i < n_tot; ++i)
	{
		logger_particle* p = &(particles[i]);
		data.push_back({{p->pos[0], p->pos[1], p->pos[2]},
		                timeBegin,
		                p->type,
		                {0.f, 0.f, 0.f},
		                timeBegin - 1e-4,
		                p->type});

		for(unsigned int i(0); i < 3; ++i)
		{
			if(p->pos[i] < min[i])
				min[i] = p->pos[i];
			if(p->pos[i] > max[i])
				max[i] = p->pos[i];
		}
		if(p->pos[i] < tmin)
			tmin = p->pos[i];
		if(p->pos[i] > tmax)
			tmax = p->pos[i];

		timeSorted[timeBegin - 1e-4].push_back(i);
	}

	shader = GLHandler::newShader("particles");
	mesh   = GLHandler::newMesh();
	GLHandler::setVertices(mesh, &(data[0].pos_prev[0]), 10 * n_tot, shader,
	                       {{"pos_prev", 4},
	                        {"color_prev", 1},
	                        {"pos_next", 4},
	                        {"color_next", 1}});

	update(timeBegin);
}

void Particles::update(double time)
{
	if(time > timeEnd)
		return;
	this->time = time;
	o          = logger_reader_get_next_offset_from_time(reader, time);

	for(unsigned int i(0); i < threadsNumber; ++i)
	{
		workerThreads[i] = new WorkerThread(
		    (i + threadsNumber * currentPortion) * data.size()
		        / (portionsNumber * threadsNumber),
		    (i + 1 + threadsNumber * currentPortion) * data.size()
		        / (portionsNumber * threadsNumber),
		    reader, &data, particles, &time, &o);
		workerThreads[i]->start();
	}
	for(unsigned int i(0); i < threadsNumber; ++i)
	{
		workerThreads[i]->wait(INT_MAX);
		delete workerThreads[i];
	}
	++currentPortion;
	currentPortion %= portionsNumber;

	/*
	for(size_t i(0); i < data.size(); ++i)
	{
	    if(time > data[i].time_next)
	    {
	        logger_particle next;
	        logger_reader_get_next_particle(reader, &(particles[i]), &next, o);

	        data[i].pos_prev[0] = particles[i].pos[0];
	        data[i].pos_prev[1] = particles[i].pos[1];
	        data[i].pos_prev[2] = particles[i].pos[2];
	        data[i].time_prev   = particles[i].time;

	        data[i].pos_next[0] = next.pos[0];
	        data[i].pos_next[1] = next.pos[1];
	        data[i].pos_next[2] = next.pos[2];
	        data[i].time_next   = next.time;
	        timeSorted[next.time].push_back(i);
	    }
	}*/
	GLHandler::updateVertices(mesh, &(data[0].pos_prev[0]), 10 * data.size());
}

void Particles::render(double time)
{
	QMatrix4x4 scale;

	scale.translate(0.5, 0.5, 0.5);
	scale.scale(100.0);
	scale.translate(-0.5, -0.5, -0.5);
	scale.scale(1.0 / 4564.667500421165);

	GLHandler::beginTransparent();
	GLHandler::setShaderParam(shader, "time", (float) time);
	GLHandler::setUpRender(shader, scale);
	GLHandler::render(mesh, GLHandler::PrimitiveType::POINTS);
	GLHandler::endTransparent();
}

Particles::~Particles()
{
	delete particles;
	logger_reader_free(reader);
	delete reader;

	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);
}

std::ostream& operator<<(std::ostream& out, Particles::Particle const& p)
{
	out << "{" << std::endl << "Prev :" << std::endl;
	out << p.pos_prev[0] << " " << p.pos_prev[1] << " " << p.pos_prev[2] << " "
	    << p.time_prev << std::endl;
	out << "Next :" << std::endl;
	out << p.pos_next[0] << " " << p.pos_next[1] << " " << p.pos_next[2] << " "
	    << p.time_next << std::endl
	    << "}";

	return out;
}

void WorkerThread::run()
{
	for(size_t i(begin); i < end; ++i)
	{
		if(*time > (*data)[i].time_next)
		{
			logger_particle next;
			logger_reader_get_next_particle(reader, &(particles[i]), &next,
			                                *offset);

			(*data)[i].pos_prev[0] = particles[i].pos[0];
			(*data)[i].pos_prev[1] = particles[i].pos[1];
			(*data)[i].pos_prev[2] = particles[i].pos[2];
			(*data)[i].time_prev   = particles[i].time;
			(*data)[i].color_prev  = particles[i].type / 4;

			(*data)[i].pos_next[0] = next.pos[0];
			(*data)[i].pos_next[1] = next.pos[1];
			(*data)[i].pos_next[2] = next.pos[2];
			(*data)[i].time_next   = next.time;
			(*data)[i].color_next  = next.type / 4;
		}
	}
}
