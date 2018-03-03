#pragma once

#include "Particle.hpp"

class ParticleSystem
{
public:
	ParticleSystem(int max_particles);
	~ParticleSystem();
	void update(float dt);
private:
	Particle* m_particle_pool;
	int m_max_particles;
	int m_num_alive_particles;
};

