#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon food
class Bullet : public Sprite
{
public:
	Bullet();

	void set_on_beat(bool was_created_on_beat){created_on_beat = was_created_on_beat;};

	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	vec2 m_movement_dir;
	
	float get_damage(){return (created_on_beat ? bullet_damage * 4 : bullet_damage);};

private:
	float bullet_damage = 25.0f;
	bool created_on_beat;
};
