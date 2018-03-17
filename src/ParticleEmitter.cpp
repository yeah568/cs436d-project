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

void ParticleEmitter::update(int elapsed_ms) {
	int i;
	for (i = 0; i < m_num_alive_particles; i++) {
		m_particle_pool[i].update(elapsed_ms);
		if (!m_particle_pool[i].getIsAlive()) {
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

	// Loading shaders
	effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl"));

	int i = 0;
	while (i < m_max_particles) {
		m_particle_pool[i++].init(m_position, 500, rand() % 359, rand() % 3 + 3);
		m_num_alive_particles++;
	}

	if (gl_has_errors()) {
		return false;
	}


	return true;
}

void ParticleEmitter::draw(const mat3& projection) {
	if (m_num_alive_particles > 0) {
		// Setting vertices and indices
		glBindVertexArray(m_particle_pool[0].mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_particle_pool[0].mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_particle_pool[0].mesh.ibo);

		// Setting shaders
		glUseProgram(effect.program);


		for (int i = 0; i < m_num_alive_particles; i++) {
			m_particle_pool[i].draw(projection, effect);
		}
	}
}

int ParticleEmitter::get_alive_particles() {
	return m_num_alive_particles;
}