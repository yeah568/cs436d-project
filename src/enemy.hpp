#pragma once

#include "common.hpp"
#include "bullet.hpp"

// Salmon enemy 
class Enemy : public Renderable
{
	// Shared between all enemys, no need to load one for each instance
	static Texture enemy_texture;
	

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Update enemy due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	void die() { alive = false; };

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current enemy position
	vec2 get_position()const;

	// Sets the new enemy position
	void set_position(vec2 position);

	// Returns the enemy' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	bool collides_with(const Bullet& bullet);
	
	static vec2 player_position;

	static void update_player_position(vec2 pos) { player_position = pos; };

	
private:
	bool alive;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};