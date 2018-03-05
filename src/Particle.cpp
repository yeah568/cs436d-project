#include "Particle.hpp"


Particle::Particle() {
	m_position = { 0, 0 };
	m_lifespan = 0;
	m_velocity = { 0, 0 };
}

void Particle::init(vec2 position, float lifespan, float angle, float speed)
{
	m_position = position;
	m_lifespan = lifespan;
	float angleRads = angle * 3.14 / 180;
	m_velocity.x = speed * cos(angleRads);
	m_velocity.y = -speed * sin(angleRads);
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