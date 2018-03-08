#include "Boss.hpp"

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
	m_position = { 600.f, 180.f };
	m_rotation = 0.f;
}

bool Boss::init(float health, std::vector<LittleEnemy>* little_enemies,
std::unordered_map<std::string, Texture*>* textures, std::vector<Structure*>* structures) 
{
	m_health = health;
	total_health = health;
	m_little_enemies = little_enemies;
	m_textures = textures;
	m_structures = structures;

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

void Boss::on_beat(Beat* beat, vec2 screen) {
	int action;
	if (m_structures->size() < 3)
		action = rand() % 5;
	else
		action = rand() % 3;

	switch (action) {
	case 0:
		move({ -10.f, 0.f });
		break;
	case 1:
		move({ 10.f, 0.f });
		break;
	case 2: {
		LittleEnemy little_enemy;
		little_enemy.set_texture((*m_textures)["enemy0"]);
		if (little_enemy.init()) {
			little_enemy.set_position(
				{ ((64.f + (float)beat->x) / 640.f) * screen.x, ((48.f + (float)beat->y) / 480.f) * screen.y * 0.67f });
			m_little_enemies->emplace_back(little_enemy);
		}
	}
		break;
	case 3: {
		return;
		Healing_Structure* new_structure = new Healing_Structure();
		new_structure->set_texture((*m_textures)["enemy0"]);
		if (!(new_structure->init())) {
			printf("Issues\n");
			return;
		}
		new_structure->set_position({screen.x/4.f*(1+m_structures->size()), 300.f});
		new_structure->set_scale({1.f, 1.f});
		new_structure->set_rotation(0.f);
		new_structure->set_boss(this);
		m_structures->emplace_back(new_structure);
	}
		break;
	case 4: {
		Black_Hole_Structure* new_structure = new Black_Hole_Structure();
		new_structure->set_texture((*m_textures)["enemy0"]);
		if (!(new_structure->init())) {
			printf("Issues\n");
			return;
		}
		new_structure->set_position({screen.x/4.f*(1+m_structures->size()), 300.f});
		new_structure->set_scale({0.5f, 0.5f});
		new_structure->set_rotation(0.f);
		m_structures->emplace_back(new_structure);
	}
		break;
	}
}

bool Boss::collides_with(const Bullet& bullet)
{
	bbox boss_bbox = get_bounding_box();
	bbox bullet_bbox = bullet.get_bounding_box();
	return bullet_bbox.min_x <= boss_bbox.max_x && bullet_bbox.max_x >= boss_bbox.min_x &&
		bullet_bbox.min_y <= boss_bbox.max_y && bullet_bbox.max_y >= boss_bbox.min_y;
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
	float rest_of_health = get_total_health() - get_health();
    m_health += std::min(delta, rest_of_health);
}
