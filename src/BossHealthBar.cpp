// Header
#include "BossHealthBar.hpp"

#include <cmath>
#include <cfloat>

BossHealthBar::BossHealthBar()
	: Sprite(nullptr)
{
	m_scale.x = 1.1f;
	m_scale.y = 0.33f;
	m_position.x = 50;
	m_position.y = 0;
}
