#pragma once
#include "common.hpp"

class Particle
{
public:
	Particle();
	void init(vec2 position, float lifespan, float angle, float speed);
	void update(float elapsed_ms);

	void copy_from(Particle & particle);

	bool is_alive;
private:
	vec2 m_position;
	vec2 m_velocity;
	float m_lifespan;
};

