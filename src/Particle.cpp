#include "Particle.hpp"
#include <math.h>

Particle::Particle() {
	m_position = { 0, 0 };
	m_lifespan = 0;
	m_velocity = { 0, 0 };
}

static const Vertex vertex_buffer_data[] = {
	{{-0.5f, -0.5f, -0.01f}, {1.f, 0.f, 0.f}},
{ { 0.5f, -0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
{ { -0.5f,  0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
{ { 0.5f,  0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
};

static const uint16_t indices[] = {
	0, 1, 2, 0, 2, 3
};

void Particle::init(vec2 position, float lifespan, float angle, float speed)
{
	m_position = position;
	m_lifespan = lifespan;
	float angleRads = angle * 3.14 / 180;
	m_velocity.x = speed * cos(angleRads);
	m_velocity.y = -speed * sin(angleRads);


	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertex_buffer_data, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Loading shaders
	effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl"));

}


void Particle::update(float elapsed_ms) {
	m_lifespan -= elapsed_ms;
	if (m_lifespan < 0) {
		is_alive = false;
		return;
	}

	m_position.x += m_velocity.x;
	m_position.y += m_velocity.y;
}

void Particle::copy_from(Particle& particle) {
	m_position.x = particle.m_position.x;
	m_position.y = particle.m_position.y;
	m_velocity.x = particle.m_velocity.x;
	m_velocity.y = particle.m_velocity.y;
	is_alive = particle.is_alive;
	m_lifespan = particle.m_lifespan;
}

vec2 Particle::getPosition() {
	return m_position;
}


void Particle::draw(const mat3& projection) {
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
	float color[] = { 1.f, 0.f, 0.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);


	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}