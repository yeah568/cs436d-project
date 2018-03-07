// Header
#include "Bullet.hpp"

#include <cmath>
#include <cfloat>

Bullet::Bullet()
  : Sprite(nullptr)
{
  m_scale.x = -1.1f;
  m_scale.y = 1.1f;
  m_position.x = -50;
  m_position.y = 50;
}

bool Bullet::init(float dmg, float spd)
{
	m_damage = dmg;
	m_speed = spd;
	return Sprite::init();
}

void Bullet::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float step = m_speed * (ms / 1000);
	
	m_position.x += m_movement_dir.x*step;
	m_position.y += m_movement_dir.y*step;
}

float Bullet::get_damage()
{
	return m_damage;
}

void Bullet::set_damage(float dmg)
{
	m_damage = dmg;
}

float Bullet::get_speed()
{
	return m_speed;
}

void Bullet::set_speed(float spd)
{
	m_speed = spd;
}
