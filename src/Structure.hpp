#pragma once
#ifndef Structure_H
#define Structure_H

#include "common.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Sprite.hpp"
#include "Boss.hpp"

class Boss;

class Structure : public Sprite {

public:
	static Player* player;
	static std::vector<Bullet>* player_bullets;

	Structure();
	
	// ms represents the number of milliseconds elapsed from the previous update() call
	virtual void update(float ms);

	bool collides_with(const Bullet &bullet);

	void scale_by(float scale);
	
	int health;


private:
	
};

class Healing_Structure : public Structure {

public:
	//Level1(int width, int height) : Level(width, height) {};
	//Healing_Structure(vec2 pos, vec2 scale, float rot, Texture* texture) : Sprite(pos, scale, rot, texture) {};
	void update(float ms);
	void set_boss(Boss* boss) {b = boss;};
private:
	Boss* b;
	void heal_boss(float ms);
};

class Shooting_Structure : public Structure {
public:
	void update(float ms);
private:
	void shoot();
	float time_until_shoot;
}

;

class Black_Hole_Structure : public Structure {
public:
	
	bool init();
	void update(float ms);
	float get_time_until_expiry() {return time_until_expiry;};
private:
	float time_until_expiry;
	float const GRAVITY_CONST = 1.f;
};
#endif
