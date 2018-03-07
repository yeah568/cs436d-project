#include "ParticleEmitter.hpp"

#include <stdlib.h>
#include <vector>

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
	if (gl_has_errors()) {
		return false;
	}

	// Loading shaders
	if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl"))) {
		return false;
	}

	return true;
}

void ParticleEmitter::draw(const mat3& projection) {
	Vertex* vertices = new Vertex[m_num_alive_particles];
	uint16_t* indices = new uint16_t[m_num_alive_particles];

	// Reading vertices and colors
	size_t num_particles;
	for (size_t i = 0; i < m_num_alive_particles; i++) {
		vec2 pos = m_particle_pool[i].getPosition();
		
		vertices[i * 4].position = { pos.x - 0.5, pos.y - 0.5, -0.01f };
		vertices[i * 4].color = { (float)255 / 255, (float)0 / 255, (float)0 / 255 };

		vertices[i * 4 + 1].position = { pos.x - 0.5, pos.y + 0.5, -0.01f };
		vertices[i * 4 + 1].color = { (float)255 / 255, (float)0 / 255, (float)0 / 255 };

		vertices[i * 4 + 2].position = { pos.x + 0.5, pos.y + 0.5, -0.01f };
		vertices[i * 4 + 2].color = { (float)255 / 255, (float)0 / 255, (float)0 / 255 };

		vertices[i * 4 + 3].position = { pos.y + 0.5, pos.y - 0.5, -0.01f };
		vertices[i * 4 + 3].color = { (float)255 / 255, (float)0 / 255, (float)0 / 255 };
	}

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex) * m_max_particles, vertices, GL_STATIC_DRAW);

	for (size_t i = 0; i < m_max_particles; i++) {
		indices[i * 6] = i * 4;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4;
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint16_t) * m_max_particles, indices, GL_STATIC_DRAW);


	transform_begin();
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// SALMON TRANSFORMATION CODE HERE
	// see Transformations and Rendering in the specification pdf
	// the following functions are available:
	// transform_translate()
	// transform_rotate()
	// transform_scale()
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// REMOVE THE FOLLOWING LINES BEFORE ADDING ANY TRANSFORMATION CODE
	transform_translate(m_position);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	
	transform_end();
	
	// Setting shaders
	glUseProgram(effect.program);
	
	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	
	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	
	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	
	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
	
	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	
	// !!! Salmon Color
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	
	
	// Drawing!
	glDrawElements(GL_TRIANGLES, (GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
	

	delete[] vertices;
	delete[] indices[];
}