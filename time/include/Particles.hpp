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

#ifndef PARTICLES_H
#define PARTICLES_H

#include <QElapsedTimer>
#include <cfloat>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include <QThread>

#include "GLHandler.hpp"

struct logger_reader;
struct logger_particle;

class WorkerThread;

class Particles
{
  public:
	struct Particle
	{
		float pos_prev[3];
		float time_prev;
		float color_prev;
		float pos_next[3];
		float time_next;
		float color_next;
	};

	Particles(const char* filename);
	void update(double time);
	void render(double time);
	~Particles();

	double const& timeBegin = timeBegin_;
	double const& timeEnd   = timeEnd_;

  private:
	double timeBegin_;
	double timeEnd_;

	logger_reader* reader;
	logger_particle* particles;

	std::vector<Particle> data;

	std::map<double, std::list<size_t>> timeSorted;

	// Multithreading
	WorkerThread* workerThreads[8];

	double time;
	size_t o;

	unsigned int currentHalf = 0;

	// GL
	GLHandler::ShaderProgram shader;
	GLHandler::Mesh mesh;
};

std::ostream& operator<<(std::ostream& out, Particles::Particle const& p);

class WorkerThread : public QThread
{
	Q_OBJECT
  public:
	WorkerThread(size_t begin, size_t end, logger_reader* reader,
	             std::vector<Particles::Particle>* data,
	             logger_particle* particles, double* time, size_t* offset)
	    : begin(begin)
	    , end(end)
	    , time(time)
	    , offset(offset)
	    , reader(reader)
	    , data(data)
	    , particles(particles)
	{
	}

  private:
	void run() override;

	size_t begin;
	size_t end;

	double* time;
	size_t* offset;

	logger_reader* reader;
	std::vector<Particles::Particle>* data;
	logger_particle* particles;
};

#endif // PARTICLES_H
