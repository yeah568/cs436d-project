#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon food
class HealthBar : public Sprite
{
public:
	HealthBar();
	vec2 m_movement_dir;

	void update(float percent_health);
private:
};