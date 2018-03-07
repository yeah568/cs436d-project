#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon food
class Bullet : public Sprite
{
public:
	Bullet();

	bool init(float dmg, float spd);

	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	vec2 m_movement_dir;
	
	float get_damage();

	void set_damage(float dmg);

	float get_speed();
	
	void set_speed(float spd);

private:
	float m_damage = 25.0f;
	float m_speed = 800.f;
};
