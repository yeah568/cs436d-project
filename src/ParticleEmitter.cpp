#include "ParticleEmitter.hpp"

#include <stdlib.h>
#include <vector>

ParticleEmitter::ParticleEmitter(vec2 position, int max_particles, bool continuous_repeat)
{
	m_max_particles = max_particles;
	m_num_alive_particles = 0;
	m_particle_pool = new Particle[max_particles];
	m_position = position;
	m_continuous_repeat = continuous_repeat;
}

ParticleEmitter::~ParticleEmitter() {
	delete[] m_particle_pool;
}

void ParticleEmitter::update(float elapsed_ms) {
	int i;
	for (i = 0; i < m_num_alive_particles; i++) {
		m_particle_pool[i].update(elapsed_ms);
		if (!m_particle_pool[i].is_alive) {
			// replace dead particle with new 
			m_particle_pool[i] = m_particle_pool[--m_num_alive_particles];
		}
	}
}

bool ParticleEmitter::init()
{
	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	
	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	int i = 0;
	while (i < m_max_particles) {
		m_particle_pool[i++].init(m_position, 500.f, (rand() % 35999) / 100.f, rand() % 3 + 3);
		m_num_alive_particles++;
	}

	if (gl_has_errors()) {
		return false;
	}


	return true;
}

void ParticleEmitter::draw(const mat3& projection) {
	for (int i = 0; i < m_num_alive_particles; i++) {
		m_particle_pool[i].draw(projection);
	}
}

int ParticleEmitter::get_alive_particles() {
	return m_num_alive_particles;
}