#pragma once
#include "common.hpp"

class Particle : public Renderable
{
public:
	Particle();
	void init(vec2 position, float lifespan, float angle, float speed);
	void update(float elapsed_ms);

	void copy_from(Particle & particle);

	vec2 getPosition();

	void draw(const mat3 & projection);

	bool is_alive;
private:
	vec2 m_position;
	vec2 m_velocity;
	float m_lifespan;
	float m_size;
};

