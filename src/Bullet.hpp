#pragma once

#include "common.hpp"
#include <vector>
#include "Sprite.hpp"

// Salmon food
class Bullet : public Sprite
{
public:
	Bullet();

	bool init(float dmg, float spd);

	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	vec2 m_movement_dir;
	
	float get_damage();

	void set_damage(float dmg);

	float get_speed();

	void set_velocity(vec2 v);
	
	void set_speed(float spd);

protected:
	vec2 velocity;
	float m_damage = 25.0f;
	float m_speed = 800.f;
};

class PlayerBullet : public Bullet
{
public:
	PlayerBullet() : Bullet() {};

	static Texture* type_1;
	static Texture* type_2;

	void addForce(force delta) {added_forces.emplace_back(delta);};

	void update(float ms);

private:
	std::vector<force> added_forces;
	force get_net_force();
};

class EnemyBullet : public Bullet
{
public:

	EnemyBullet() : Bullet() {};
	static Texture* texture;
private:
	
};