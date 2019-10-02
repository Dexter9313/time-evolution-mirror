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

#include <iostream>
#include <vector>

#include "GLHandler.hpp"

struct logger_reader;
struct logger_particle;

class Particles
{
  public:
	struct Particle
	{
		float pos_prev[3];
		float time_prev;
		float pos_next[3];
		float time_next;
	};

	Particles(const char* filename);
	void update(double time);
	void render(double time);
	~Particles();

	double const& timeBegin = timeBegin_;
	double const& timeEnd = timeEnd_;

  private:
	double timeBegin_;
	double timeEnd_;

	logger_reader* reader;
	logger_particle* particles;

	std::vector<Particle> data;

	// GL
	GLHandler::ShaderProgram shader;
	GLHandler::Mesh mesh;
};

std::ostream& operator<<(std::ostream& out, Particles::Particle const& p);

#endif // PARTICLES_H
