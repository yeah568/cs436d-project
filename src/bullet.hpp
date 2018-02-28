#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon food
class Bullet : public Sprite
{
public:
  Bullet();

	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	vec2 m_movement_dir;
};
