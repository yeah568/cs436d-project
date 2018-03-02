#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon food
class BossHealthBar : public Sprite
{
public:
	
	
	BossHealthBar();
	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void set_health_percentage(float percentage){m_scale.x = percentage * screen_scale;};

	

private:
	
	float screen_scale = 0.5f;
};