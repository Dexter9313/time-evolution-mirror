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

	std::cout << "Time begin: " << timeBegin << " end: " << timeEnd
	          << std::endl;
	logger_reader_set_time(reader, timeBegin);

	/* Get the number of particles */
	int n_type      = 0;
	long long n_tot = 0;
	const long long* n_parts
	    = logger_reader_get_number_particles(reader, &n_type);
	for(int i = 0; i < n_type; i++)
	{
		n_tot += n_parts[i];
		printf("%lld particles out of %lld...\n", n_parts[i], n_tot);
	}

	particles = new logger_particle[n_tot];

	logger_reader_read_from_index(reader, timeBegin, logger_reader_const,
	                              particles, n_tot);

	for(int i(0); i < n_tot; ++i)
	{
		logger_particle* p = &(particles[i]);
		data.push_back({{p->pos[0], p->pos[1], p->pos[2]},
		                timeBegin,
		                {0.f, 0.f, 0.f},
		                timeBegin - 1e-4});
	}

	shader = GLHandler::newShader("particles");
	GLHandler::setShaderParam(shader, "color", QColor(255, 255, 255));
	GLHandler::setShaderParam(shader, "alpha", 1.f);
	mesh = GLHandler::newMesh();
	GLHandler::setVertices(mesh, &(data[0].pos_prev[0]), 8*n_tot, shader, {{"pos_prev", 4}, {"pos_next", 4}});

	update(timeBegin);
}

void Particles::update(double time)
{
	size_t o = logger_reader_get_offset_from_time(reader, time);
	for(unsigned int i(0); i < data.size(); ++i)
	{
		if(time > data[i].time_next)
		{
			logger_particle next;
			logger_reader_get_next_particle(reader, &(particles[i]), &next, o);

			data[i].pos_prev[0] = particles[i].pos[0];
			data[i].pos_prev[1] = particles[i].pos[1];
			data[i].pos_prev[2] = particles[i].pos[2];
			data[i].time_prev = particles[i].time;

			data[i].pos_next[0] = next.pos[0];
			data[i].pos_next[1] = next.pos[1];
			data[i].pos_next[2] = next.pos[2];
			data[i].time_next = next.time;

			if(i == 0)
				std::cout << data[i] << std::endl;
		}
	}
	GLHandler::updateVertices(mesh, &(data[0].pos_prev[0]), 8 * data.size());

}

void Particles::render(double time)
{
	GLHandler::setShaderParam(shader, "time", (float)time);
	GLHandler::setUpRender(shader);
	GLHandler::render(mesh, GLHandler::PrimitiveType::POINTS);
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