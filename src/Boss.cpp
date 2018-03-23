#include "Boss.hpp"
#include "Hitbox.hpp"
#include "Spawner.hpp"
#include "common.hpp"

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
	structure_slots.left = nullptr;
	structure_slots.center = nullptr;
	structure_slots.right = nullptr;
}

bool Boss::init(float health, std::vector<LittleEnemy>* little_enemies, std::vector<Structure*>* structures) 
{
	m_health = health;
	total_health = health;
	m_little_enemies = little_enemies;
	Hitbox* hitbox = new Hitbox();
	if (hitbox->init({ 0.f, 80.f }, { 0.8f, 0.6f }, 0.f, this, m_texture)) {
		m_hitboxes.push_back(hitbox);
	}
	m_structures = structures;
	tm = TextureManager::get_instance();
	return Sprite::init();
}

void Boss::update(float ms, vec2 screen, std::vector<PlayerBullet>* bullets)
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

void Boss::set_slot(vec2 screen, Structure* structure) {
    if (structure_slots.left == nullptr) {
        structure_slots.left = structure;
        structure->set_position({screen.x/4.f, 300.f});
    //} else if (structure_slots.center == nullptr) {
     //   structure_slots.center = structure;
      //  structure->set_position({screen.x/4.f*2, 300.f});
    } else {
        structure_slots.right = structure;
        structure->set_position({screen.x/4.f*3, 300.f});
    }
}

void Boss::on_beat(Beat* beat, vec2 screen) {
	int action;
	if (m_structures->size() < 2)
		action = rand() % 6;
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
		vec2 position = { ((64.f + (float)beat->x) / 640.f) * screen.x,
			((48.f + (float)beat->y) / 480.f) * screen.y * 0.67f };
		spawn_little_enemy(position, tm->get_texture("enemy0"), m_little_enemies);
	}
		break;
	case 3: {
		vec2 position = { screen.x / 4.f, 300.f };
		
			
		spawn_structure(HEALING_STRUCTURE, this, position, tm->get_texture("enemy0"), m_structures);
		Structure* just_added = m_structures->back();
        set_slot(screen, just_added);
	}
		break;
	case 4: {
		vec2 position = { screen.x / 4.f, 300.f };
		
		spawn_structure(BLACK_HOLE_STRUCTURE, this, position, tm->get_texture("enemy0"), m_structures);
        Structure* just_added = m_structures->back();
        set_slot(screen, just_added);
	}
		break;
	case 5: {
		vec2 position;
		position = { screen.x / 4.f, 300.f };
		
		spawn_structure(SHOOTING_STRUCTURE, this, position, tm->get_texture("enemy1"), m_structures);
        Structure* just_added = m_structures->back();
        set_slot(screen, just_added);
	}
		printf("Finished boss spawning structure\n");
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
	float rest_of_health = get_total_health() - get_health();
    m_health += std::min(delta, rest_of_health);
}
