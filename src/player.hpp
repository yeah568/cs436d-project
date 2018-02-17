#pragma once

#include "common.hpp"

class Turtle;
class Fish;

class Player : public Renderable
{
	
public:
	// Creates all the associated render resources and default transform
	bool init();
	static Texture player_texture;

	// Releases all associated resources
	void destroy();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the salmon
	void draw(const mat3& projection)override;

	// Collision routines for turtles and fish
	bool collides_with(const Turtle& turtle);
	bool collides_with(const Fish& fish);

	// Returns the current salmon position
	vec2 get_position()const;

	float get_rotation()const;

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	// Set salmon rotation in radians
	void set_rotation(float radians);

	void set_mouse(float x, float y);

	// True if the salmon is alive
	bool is_alive()const;

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// Callesd when the salmon collides with a fish, starts lighting up the salmon
	void light_up();
	
	void dash();

	void set_scale(vec2 scale);

	void set_movement_dir(vec2 dir);

	void add_movement_dir(vec2 dir);

	float exploding_timer;

	bool bullet_type;

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	bool m_is_alive; // True if the salmon is alive
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	vec2 m_movement_dir;
	vec2 m_mouse;
};