// Header
#include "HealthBar.hpp"

#include <cmath>
#include <cfloat>

HealthBar::HealthBar()
	: Sprite(nullptr)
{
	m_scale = { 1.f, 1.f };
	m_position = { 50, -50 };
}

void HealthBar::update(float percent_health)
{
	set_scale({ percent_health, 1.0f });
	bbox hp_bb = get_bounding_box();
	set_position({ (hp_bb.max_x - hp_bb.min_x) / 2.0f,200.0f });
	float r = 1.0f;
	float g = std::max(percent_health, 0.0f);
	set_color(r*2.0f, g, g);
}
