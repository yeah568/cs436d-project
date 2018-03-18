#include "Particle.hpp"
#include <math.h>

Particle::Particle() {
	m_position = { 0, 0 };
	m_lifespan = 0;
	m_velocity = { 0, 0 };
	is_alive = true;
}



void Particle::init(vec2 position, float lifespan, float angle, float speed)
{
	m_position = position;
	m_lifespan = lifespan;
	float angleRads = angle * 3.14 / 180;
	m_velocity.x = speed * cos(angleRads);
	m_velocity.y = -speed * sin(angleRads);
	m_size = 5.f;
}

static const float p = 1.293f;
static const float A = 0.1f;
static const float Cd = 0.47f;
static const float m = 1.f;

void Particle::update(float elapsed_ms) {
	m_lifespan -= elapsed_ms;
	if (m_lifespan < 0) {
		is_alive = false;
		return;
	}


	float fd_x = -m_velocity.x * 0.5f * p * Cd * A * pow(m_velocity.x, 2);
	float fd_y = -m_velocity.y * 0.5f * p * Cd * A * pow(m_velocity.y, 2);
	float a_x = fd_x / m;
	float a_y = fd_y / m;

	m_velocity.x += a_x * (elapsed_ms / 1000);
	m_velocity.y += a_y * (elapsed_ms / 1000);

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

bool Particle::getIsAlive() {
	return is_alive;
}

void Particle::draw(const mat3& projection) {
	return;
}

void Particle::draw(const mat3& projection, Effect& emitter_effect) {
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
	transform_scale({ m_size, m_size });
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	transform_end();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(emitter_effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(emitter_effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(emitter_effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(emitter_effect.program, "projection");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}