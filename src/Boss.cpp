#include "Boss.hpp"
#include "Hitbox.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <cfloat>

Boss::Boss() 
	: Sprite(nullptr)
{

	// Setting initial values
	m_scale.x = 0.75;
	m_scale.y = 0.75;
	m_position = { 600.f, 80.f };
	m_rotation = 0.f;
}

bool Boss::init(float health, std::vector<LittleEnemy>* little_enemies)
{
	m_health = health;
	m_little_enemies = little_enemies;
	Hitbox* hitbox = new Hitbox();
	if (hitbox->init({ 0.f, 80.f }, { 0.8f, 0.6f }, 0.f, this, m_texture)) {
		m_hitboxes.push_back(hitbox);
	}
	return Sprite::init();
}

void Boss::update(float ms, vec2 screen, std::vector<Bullet>* bullets)
{
	// Very naive bullet avoidance algorithm.
	// On each update, get the highest up bullet (oldest on bullet array)
	// and go the other direction only if the bullet will hit it.
	float boss_speed = 500.0f;
	float step = ms / 1000.0f;
	if (bullets->size() > 0) {
		auto top_bullet = bullets->at(0);
		auto top_bullet_bb = top_bullet.get_bounding_box();
		auto dist_left = top_bullet.get_position().x;
		auto dist_right = screen.x - dist_left;
		bbox bb = get_bounding_box();
		if (top_bullet_bb.min_x >= bb.max_x || top_bullet_bb.max_x <= bb.min_x) {
			return;
		}

		if (dist_left > dist_right) {
			move({ -boss_speed*step, 0.f });
			if (bb.min_x < 0) { m_position.x = (bb.max_x - bb.min_x) / 2; }
		}
		else {
			move({ boss_speed*step, 0.f });
			if (bb.max_x > screen.x) { m_position.x = screen.x - (bb.max_x - bb.min_x) / 2; }
		}
	}

}

void Boss::on_beat(Beat* beat, vec2 screen, Texture* enemy_texture) {
	int action = rand() % 3;

	switch (action) {
	case 0:
		move({ -20.f, 0.f });
		break;
	case 1:
		move({ 20.f, 0.f });
		break;
	case 2:
		LittleEnemy little_enemy;
		little_enemy.set_texture(enemy_texture);
		if (little_enemy.init()) {
			little_enemy.set_position(
				{ ((64.f + (float)beat->x) / 640.f) * screen.x, ((48.f + (float)beat->y) / 480.f) * screen.y * 0.67f });
			m_little_enemies->emplace_back(little_enemy);
		}
		break;
	}
}

void Boss::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

float Boss::get_health()const
{
    return m_health;
}

void Boss::set_health(float delta)
{
    m_health += delta;
}
