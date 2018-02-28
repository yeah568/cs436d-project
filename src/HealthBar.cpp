// Header
#include "HealthBar.hpp"

#include <cmath>
#include <cfloat>

HealthBar::HealthBar()
	: Sprite(nullptr)
{
	m_scale.x = 1.1f;
	m_scale.y = 1.1f;
	m_position.x = 50;
	m_position.y = -50;
}

void HealthBar::update()
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	
}
