#pragma once

#ifndef ParticleEmitter_H
#define ParticleEmitter_H

#include "Particle.hpp"
#include <vector>

class ParticleEmitter : public Renderable
{
public:
	ParticleEmitter(vec2 position, int max_particles, bool continuous_repeat);

	~ParticleEmitter();

	void update(int elapsed_ms);
	bool init();
	void draw(const mat3 & projection);
	int get_alive_particles();
private:
	std::vector<std::shared_ptr<Particle>> m_particle_pool;
	bool m_should_cull;
	int m_max_particles;
	int m_num_alive_particles;
	vec2 m_position;
	bool m_continuous_repeat;
};

#endif
