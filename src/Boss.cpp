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
    } else if (structure_slots.center == nullptr) {
        structure_slots.center = structure;
        structure->set_position({screen.x/4.f*2, 300.f});
    } else {
        structure_slots.right = structure;
        structure->set_position({screen.x/4.f*3, 300.f});
    }
}

void Boss::remove_structure(Structure* s, float current_time)
{
	if (Healing_Structure* h = dynamic_cast<Healing_Structure*>(s)) {
		num_healing--;
	}
	else if (Shooting_Structure* h = dynamic_cast<Shooting_Structure*>(s)) {
		num_shooting--;
	}
	else if (Black_Hole_Structure* h = dynamic_cast<Black_Hole_Structure*>(s)) {
		num_black_hole--;
	}
	last_structure_destroyed_time = current_time;
}

void Boss::on_beat(Beat* beat, vec2 screen) {
	Decision action = make_decision(beat);

	switch (action) {
	case move_l:
		move({ -10.f, 0.f });
		break;
	case move_r:
		move({ 10.f, 0.f });
		break;
	case spawn_enemy: {
		vec2 position = { ((64.f + (float)beat->x) / 640.f) * screen.x,
			((48.f + (float)beat->y) / 480.f) * screen.y * 0.67f };
		spawn_little_enemy(position, tm->get_texture("enemy0"), m_little_enemies);
	}
		break;
	case spawn_heal: {
		vec2 position = { screen.x / 4.f*(1 + m_structures->size()), 300.f };
		spawn_structure(HEALING_STRUCTURE, this, position, tm->get_texture("enemy0"), m_structures);
		Structure* just_added = m_structures->back();
		set_slot(screen, just_added);
		num_healing++;
		last_structure_spawned_time = beat->absoluteOffset;
	}
		break;
	case spawn_black_hole: {
		vec2 position = { screen.x / 4.f*(1 + m_structures->size()), 300.f };
		spawn_structure(BLACK_HOLE_STRUCTURE, this, position, tm->get_texture("enemy0"), m_structures);
		Structure* just_added = m_structures->back();
		set_slot(screen, just_added);
		num_black_hole++;
		last_structure_spawned_time = beat->absoluteOffset;
	}
		break;
	case spawn_shoot: {
		vec2 position = { screen.x / 4.f*(1 + m_structures->size()), 300.f };
		spawn_structure(SHOOTING_STRUCTURE, this, position, tm->get_texture("enemy0"), m_structures);
		Structure* just_added = m_structures->back();
		set_slot(screen, just_added);
		num_shooting++;
		last_structure_spawned_time = beat->absoluteOffset;
	}
		break;
	}
}

Decision Boss::make_decision(Beat* beat)
{
	int weights[DECISIONS_MAX];
	int num_structures = m_structures->size();
	weights[move_l] = 10 * (num_structures + 1);
	weights[move_r] = 10 * (num_structures + 1);
	weights[spawn_enemy] = 10 * (num_structures + 2);

	int base_structure_weight = 20;
	if (num_structures >= 3 ||
		beat->absoluteOffset < last_structure_destroyed_time + STRUCTURE_DESTROYED_COOLDOWN ||
		beat->absoluteOffset < last_structure_spawned_time + STRUCTURE_SPAWN_COOLDOWN) {
		if (num_structures < 3) {
			float cooldown = beat->absoluteOffset - std::max(last_structure_destroyed_time, last_structure_spawned_time);
			printf("Cooldown: %f\n", cooldown);
		}
		// If structures are full or on cooldown, set weight to 0
		base_structure_weight = 0;
		weights[spawn_heal] = base_structure_weight;
		weights[spawn_black_hole] = base_structure_weight;
		weights[spawn_shoot] = base_structure_weight;
	}
	else {
		// Add weight to structures based on how long it's been since last structure spawned
		base_structure_weight += (beat->absoluteOffset - last_structure_spawned_time) / 1000;
		float health_percent = m_health / total_health;
		// Weigh healing more when health is low
		weights[spawn_heal] = num_healing < MAX_HEALING_STRUCTURES ? base_structure_weight + (1 - health_percent) * 5 : 0;
		weights[spawn_black_hole] = num_black_hole < MAX_BLACK_HOLES ? base_structure_weight : 0;
		weights[spawn_shoot] = num_shooting < MAX_SHOOTING_STRUCTURES ? base_structure_weight : 0;
	}

	int weight_sums = 0;
	printf("Decision weights: ");
	for (int i = 0; i < DECISIONS_MAX; i++) {
		printf("%d ", weights[i]);
		weight_sums += weights[i];
	}
	printf("\n");
	int random = rand() % weight_sums;
	for (int i = 0; i < DECISIONS_MAX; i++) {
		if (random < weights[i]) {
			printf("Decision made: %d\n", i);
			return (Decision) i;
		}
		random -= weights[i];
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
