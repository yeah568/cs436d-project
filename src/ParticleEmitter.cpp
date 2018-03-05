#include "ParticleEmitter.hpp"

#include <stdlib.h>

ParticleEmitter::ParticleEmitter(vec2 position, int max_particles, bool continuous_repeat)
{
	m_max_particles = max_particles;
	m_particle_pool = new Particle[max_particles];
	m_position = position;
}

ParticleEmitter::~ParticleEmitter() {
	delete[] m_particle_pool;
}

void ParticleEmitter::update(int elapsed_ms) {
	int i;
	for (i = 0; i < m_num_alive_particles; i++) {
		m_particle_pool[i].update(elapsed_ms);
		if (!m_particle_pool[i].is_alive) {
			// replace dead particle with new 
			m_particle_pool[i].copy_from(m_particle_pool[--m_num_alive_particles]);
		}
	}

	while (i < m_max_particles) {
		m_particle_pool[i++].init(m_position, 300, rand() % 359, rand() % 3 + 3);
	}
}