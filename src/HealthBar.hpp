#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon food
class HealthBar : public Sprite
{
public:
	
	
	HealthBar();
	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update();

	vec2 m_movement_dir;

	

private:
	
	
};