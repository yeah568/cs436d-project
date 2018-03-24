#include "Player.hpp"

#include "Bullet.hpp"
#include "common.hpp"

#include <math.h>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

Player::Player()
  : Sprite(nullptr)
{
}

bool Player::init() {
	Sprite::init();
	powerUp1_time = 0;
	powerUp2_time = 0;
	powerUp3_time = 0;
	
	m_scale.x = -1.f;
	m_scale.y = 1.f;
	m_is_alive = true;
	m_position = { 600.f, 700.f };
	m_rotation = 0.f;
	m_light_up_countdown_ms = -1.f;
	m_movement_dir = { 0.f, 0.f };
	bullet_type = false;
	m_health = 0;
	
	return true;
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	const float SALMON_SPEED = 200.f;
	float step = SALMON_SPEED * (ms / 1000);
	if (m_is_alive)
	{
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// UPDATE SALMON POSITION HERE BASED ON KEY PRESSED (World::on_key())
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		vec2 normalized_movement = m_movement_dir;
		if (m_movement_dir.x != 0 && m_movement_dir.y != 0) {
			normalized_movement = normalize(normalized_movement);
		}
		
		move({ normalized_movement.x * 10, normalized_movement.y * 10 });
	}
	if (powerUp1_time > 0) {
		powerUp1_time -= ms;
	}
	if (powerUp3_time > 0) {
		powerUp3_time -= ms;
	}
	if (powerUp2_time > 0) {
		powerUp2_time -= ms;
	}

	
	printf("player scale %f", m_scale.x);
	if (std::abs(m_scale.x) > 1.f) {
		printf("adjusting scale");
		m_scale.x *= 0.99;
		m_scale.y *= 0.99;
	}
}

void Player::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Player::set_mouse(float x, float y)
{
	m_mouse = { x, y };
}

bool Player::is_alive()const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Player::kill()
{
	m_is_alive = false;
}

void Player::dash() {
	move({ m_movement_dir.x*60, m_movement_dir.y *60});
}

void Player::set_movement_dir(vec2 dir) {
	m_movement_dir = dir;
}

void Player::add_movement_dir(vec2 dir) {
	m_movement_dir.x += dir.x;
	m_movement_dir.y += dir.y;
}

void Player::set_scale(float scale) {
	m_scale.x = -scale;
	m_scale.y = scale;
}

void Player::scale_by(float scale) {
	m_scale.x *= scale;
	m_scale.y *= scale;
}
