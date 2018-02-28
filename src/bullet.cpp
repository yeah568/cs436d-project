// Header
#include "bullet.hpp"

#include <cmath>
#include "float.h"

Bullet::Bullet()
  : Sprite(nullptr)
{
  m_scale.x = -1.1f;
  m_scale.y = 1.1f;
  m_position.x = -50;
  m_position.y = 50;
}

void Bullet::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float BULLET_SPEED;
	if (m_scale.x > 1.1)
		BULLET_SPEED = 3200.f;
	else
		BULLET_SPEED = 800.f;
	float step = BULLET_SPEED * (ms / 1000);
	
	m_position.x += m_movement_dir.x*step;
	m_position.y += m_movement_dir.y*step;
}
