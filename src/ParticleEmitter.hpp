#pragma once

#include "Particle.hpp"

class ParticleEmitter : public Renderable
{
public:
	ParticleEmitter(vec2 position, int max_particles, bool continuous_repeat);

	~ParticleEmitter();

	void update(int elapsed_ms);
private:
	Particle * m_particle_pool;
	int m_max_particles;
	int m_num_alive_particles;
	vec2 m_position;
};

