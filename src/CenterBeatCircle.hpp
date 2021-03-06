#pragma once
#include "common.hpp"
#include "Player.hpp"

// Salmon food
class CenterBeatCircle : public Renderable
{
	

public:
	static Player* player;
	// Creates all the associated render resources and default transform
	bool init(bool type);

	// Releases all the associated resources
	void destroy();
	
	// Update fish due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the fish
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current fish position
	vec2 get_position()const;

	// Sets the new fish position
	void set_position(vec2 position);

	// Returns the fish' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	void set_rotation(float angle);
	vec2 m_movement_dir;
	bool beat_circle_type;
	int dir;
	void set_scale(vec2 scale);
private:
	Texture center_beat_circle_texture;

	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float SPEED;
};
