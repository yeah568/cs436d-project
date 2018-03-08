#pragma once

#include "common.hpp"
#include <vector>
#include "Sprite.hpp"

// Salmon food
class Bullet : public Sprite
{
public:
	Bullet();
	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	virtual void update(float ms) {};

	vec2 m_movement_dir;
private:
};

class PlayerBullet : public Bullet
{
public:
	PlayerBullet() : Bullet() {};

	static Texture* type_1;
	static Texture* type_2;

	void set_on_beat(bool was_created_on_beat){created_on_beat = was_created_on_beat;};
	float get_damage(){return (created_on_beat ? bullet_damage * 4 : bullet_damage);};
	void addForce(force delta) {added_forces.emplace_back(delta);};

	void update(float ms);

private:
	float bullet_damage = 25.0f;
	bool created_on_beat;
	std::vector<force> added_forces;
	force get_net_force();
};

class EnemyBullet : public Bullet
{
public:

	EnemyBullet() : Bullet() {};
	static Texture* texture;
	void update(float ms);
private:
	
};