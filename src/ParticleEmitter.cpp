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

void ParticleEmitter::draw(const mat3& projection) {
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	//printf("BC.x=%f,BC.y=%f\n", final_pos.x, final_pos.y);
	transform_begin();
	transform_translate(m_position);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniform3fv(color_uloc, 1, m_color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}