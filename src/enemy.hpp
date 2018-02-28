#pragma once

#include "common.hpp"
#include "bullet.hpp"

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
class LittleEnemy : public Renderable {
	// Shared between all little enemies, no need to load one for each instance
	static Texture little_enemy_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Update ze little enemy
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the little enemy
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	// Returns the current little enemy position
	vec2 get_position() const;

	// Sets the new little enemy position
	void set_position(vec2 position);

	// Returns the little enemy' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	bool collides_with(const Bullet &bullet);

	void scale_by(float scale);


private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};