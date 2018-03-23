#pragma once
#ifndef Boss_H
#define Boss_H

// Internal
#include "common.hpp"
#include "Bullet.hpp"
#include "BeatList.hpp"
#include "Enemy.hpp"
#include "Structure.hpp"
#include "TextureManager.hpp"

// STDLIB
#include <unordered_map>
#include <vector>

class Structure;

struct StructureSlots {
	Structure* left;
	Structure* center;
	Structure* right;
};

enum Decision { move_l, move_r, spawn_enemy, spawn_heal, spawn_black_hole, spawn_shoot, DECISIONS_MAX };

class Boss : public Sprite
{

public:
	Boss();
	// Creates all the associated render resources and default transform
	bool init(float health, std::vector<LittleEnemy>* little_enemies,
		std::vector<Structure*>* structures);

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, vec2 screen, std::vector<PlayerBullet>* bullets);

	void on_beat(Beat* beat, vec2 screen);

	void move(vec2 off);

	float get_health()const;

	void set_health(float delta);

	float get_total_health() {return total_health;};

	void remove_structure(Structure* s, float current_time);

	StructureSlots structure_slots;

	int num_healing = 0;
	int num_black_hole = 0;
	int num_shooting = 0;

	float last_structure_destroyed_time = -5000.f;
	float last_structure_spawned_time = -5000.f;

private:
	float m_health;
	float total_health;

	const int MAX_HEALING_STRUCTURES = 1;
	const int MAX_BLACK_HOLES = 2;
	const int MAX_SHOOTING_STRUCTURES = 1;
	const float STRUCTURE_SPAWN_COOLDOWN = 3000.f;
	const float STRUCTURE_DESTROYED_COOLDOWN = 5000.f;
	
	std::vector<LittleEnemy>* m_little_enemies;
	std::vector<Structure*>* m_structures;

	TextureManager* tm;

	void set_slot(vec2 screen, Structure *structure);

	Decision make_decision(Beat* beat);
};
#endif