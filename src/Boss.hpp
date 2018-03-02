#pragma once

#include "common.hpp"
#include "Bullet.hpp"
#include "BeatList.hpp"
#include "Enemy.hpp"

#include <vector>

class Boss : public Sprite
{

public:
	Boss();
	// Creates all the associated render resources and default transform
	bool init(float health, std::vector<LittleEnemy>* little_enemies);

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, vec2 screen, std::vector<Bullet>* bullets);

	void on_beat(Beat* beat, vec2 screen, Texture* enemy_texture);

	bool collides_with(const Bullet& bullet);

	void move(vec2 off);

	float get_health()const;

	void set_health(float delta);

	float get_total_health() {return total_health;};

private:
	float m_health;
	float total_health;
	std::vector<LittleEnemy>* m_little_enemies;
};