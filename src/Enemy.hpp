#pragma once

#include "common.hpp"
#include "Bullet.hpp"
#include "Player.hpp"

// TODO:
// WANT THE LITTLE ENEMIES TO SPAWN ON THE BEAT OR WHENEVER THEY ARE CALLED BY BIG BOSS (not implemented)
// WANT THE LITTLE ENEMIES TO BE DRAWN TO TOGETHER LIKE GRAVITATIONAL PULL
// WANT THE LITTLE ENEMIES TO PULSE TO THE BEAT
// WANT THE LITTLE ENEMIES TO GET BIGGER WHEN CLOSER TOGETHER AND THEN COMBINE WHEN OVERLAPPING
// WANT THE LITTLE ENEMIES TO SCATTER WHEN SHOT AT
// WANT THE LITTLE ENEMIES TO BOUNCE OFF THE WALLS
// WANT THE LITTLE ENEMIES TO TAKE HEALTH AWAY FROM PLAYER A BIT IF COLLISION
// WANT THE LITTLE ENEMIES TO ABSORB/COLLIDE WITH BULLET

// Little enemy

class Player;

class LittleEnemy : public Sprite {

public:
	static Player* player;

	LittleEnemy();

	// Update ze little enemy
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	void scale_by(float scale);

private:
	
	float MAX_ROTATION_PER_SEC = 3.14 / 6; // 30deg
};
