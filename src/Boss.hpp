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
	std::shared_ptr<Structure> left;
	std::shared_ptr<Structure> center;
	std::shared_ptr<Structure> right;
};

class Boss : public Sprite
{

public:
	Boss();
	~Boss();
	// Creates all the associated render resources and default transform
	bool init(float health, std::shared_ptr<std::vector<LittleEnemy>> little_enemies,
		std::shared_ptr<std::vector<std::shared_ptr<Structure>>> structures);

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, vec2 screen, std::shared_ptr<std::vector<PlayerBullet>> bullets);

	void on_beat(Beat& beat, vec2 screen);

	void move(vec2 off);

	float get_health()const;

	void set_health(float delta);

	float get_total_health() {return total_health;};

	StructureSlots structure_slots;

private:
	float m_health;
	float total_health;
	
	std::shared_ptr<std::vector<LittleEnemy>> m_little_enemies;
	std::shared_ptr<std::vector<std::shared_ptr<Structure>>> m_structures;

	std::shared_ptr<TextureManager> tm;

	void set_slot(vec2 screen, std::shared_ptr<Structure> structure);
};
#endif