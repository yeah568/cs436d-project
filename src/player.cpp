// Header
#include "player.hpp"

// internal
#include "turtle.hpp"
#include "bullet.hpp"
#include "common.hpp"

// stlib
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>

Player::Player()
  : Sprite(nullptr)
{
	m_scale.x = -1.f;
	m_scale.y = 1.f;
	m_is_alive = true;
	m_position = { 600.f, 700.f };
	m_rotation = 0.f;
	m_light_up_countdown_ms = -1.f;
	m_movement_dir = { 0.f, 0.f };
	bullet_type = false;
	m_health = 0;
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	const float SALMON_SPEED = 200.f;
	float step = SALMON_SPEED * (ms / 1000);
  /*
	if (exploding_timer > 0 && exploding_timer < 1.25) {
		m_scale.y = 0.4f;
		player_texture.load_from_file(textures_path("explosion_1.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 1.25 && exploding_timer < 1.5) {
		player_texture.load_from_file(textures_path("explosion_2.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 1.5 && exploding_timer < 1.75) {
		player_texture.load_from_file(textures_path("explosion_3.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 1.75 && exploding_timer < 2) {
		player_texture.load_from_file(textures_path("explosion_4.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 2 && exploding_timer < 2.25) {
		player_texture.load_from_file(textures_path("explosion_5.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 2.25 && exploding_timer < 2.5) {
		player_texture.load_from_file(textures_path("explosion_6.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 2.5 && exploding_timer < 3) {
		player_texture.load_from_file(textures_path("explosion_7.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 3 && exploding_timer <3.5) {
		player_texture.load_from_file(textures_path("explosion_8.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 3.5 && exploding_timer < 4) {
		player_texture.load_from_file(textures_path("explosion_9.png"));
		exploding_timer += ms / 1000;
	}
  */
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
	else
	{
		// If dead we make it face upwards and sink deep down
		//set_rotation(3.1415f);
		move({ 0.f, step });
	}

	if (abs(m_scale.x * 0.95) < 1.f) {
		m_scale.x = -0.5f;
	}
	else {
		m_scale.x *= 0.5;
	}

	if (abs(m_scale.y * 0.95) < 1.f) {
		m_scale.y = 0.5f;
	}
	else {
		m_scale.y *= 0.5;
	}

	if (m_light_up_countdown_ms > 0.f)
		m_light_up_countdown_ms -= ms;
}

// Simple bounding box collision check, 


bool Player::collides_with(const LittleEnemy& little_enemy)
{
	float dx = m_position.x - little_enemy.get_position().x;
	float dy = m_position.y - little_enemy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(little_enemy.get_bounding_box().x, little_enemy.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

bool Player::collides_with(const Bullet& bullet)
{
	/*float dx = m_position.x - bullet.get_position().x;
	float dy = m_position.y - bullet.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(bullet.get_bounding_box().x, bullet.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;*/
	return false;
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

// Called when the salmon collides with a fish
void Player::light_up()
{
	m_light_up_countdown_ms = 1500.f;
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