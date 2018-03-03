#pragma once

#include "Particle.hpp"

class ParticleEmitter
{
public:
	ParticleEmitter();

	void update(int elapsed_ms);
private:
	int num_particles;
	
};

