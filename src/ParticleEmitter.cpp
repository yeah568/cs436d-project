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
		if (!m_particle_pool[i].getIsAlive()) {
			// replace dead particle with new 
			m_particle_pool[i] = m_particle_pool[--m_num_alive_particles];
		}
	}
}

static const Vertex vertex_buffer_data[] = {
	{ { -0.5f, -0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
	{ { 0.5f, -0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
	{ { -0.5f,  0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
	{ { 0.5f,  0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
};

bool ParticleEmitter::init()
{
	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertex_buffer_data, GL_STATIC_DRAW);

	// Loading shaders
	effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl"));

	int i = 0;
	while (i < m_max_particles) {
		m_particle_pool[i++].init(m_position, 500, (rand() % 35999) / 100.f, (rand() % 3000) / 1000.f + 3.f);
		m_num_alive_particles++;
	}

	if (gl_has_errors()) {
		return false;
	}

	return true;
}

void ParticleEmitter::draw(const mat3& projection) {
	if (m_num_alive_particles > 0) {

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

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