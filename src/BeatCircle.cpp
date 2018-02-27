// Header
#include "BeatCircle.hpp"

#include <cmath>

BeatCircle::BeatCircle(Player* player, float speed)
  : Sprite(nullptr),
    m_player(player),
    m_speed(speed)
{
	m_scale.x = 1.1f;
	m_scale.y = 1.1f;
	m_position.x = -50;
	m_position.y = 50;
}

void BeatCircle::set_dir(int direction) {
	m_dir = direction;
	switch (m_dir) {
		case 1:
			m_movement_dir = {1.0f, 0.0f};
			break;
		case 2:
			m_movement_dir = {0.0f, -1.0f};
			break;
		case 3:
			m_movement_dir = {-1.0f, 0.0f};
			break;
		case 4:
			m_movement_dir = {0.0f, 1.0f};
			break;
	}
}

void BeatCircle::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float step = m_speed * (ms / 1);
	
	m_local_position.x += m_movement_dir.x*step;
	m_local_position.y += m_movement_dir.y*step;
  m_position = m_player->get_position() + m_local_position;
	m_scale.x -= ms/1000;
	m_scale.y -= ms/1000;
}

const vec2& BeatCircle::get_local_position() const
{
  return m_local_position;
}

int BeatCircle::get_dir() const
{
  return m_dir;
}

const vec2& BeatCircle::get_movement_dir() const
{
  return m_movement_dir;
}
