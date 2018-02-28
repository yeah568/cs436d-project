#pragma once

#include "common.hpp"
#include "LittleEnemy.hpp"
#include "Sprite.hpp"

class Turtle;
class Bullet;

class Player : public Sprite
{
	
public:
  Player();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Collision routines for turtles and fish
	bool collides_with(const Turtle& turtle);
	bool collides_with(const Bullet& bullet);
	bool collides_with(const LittleEnemy& little_enemy);

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	void set_mouse(float x, float y);

	// True if the salmon is alive
	bool is_alive()const;

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// Callesd when the salmon collides with a fish, starts lighting up the salmon
	void light_up();
	
	void dash();

	void set_movement_dir(vec2 dir);

	void add_movement_dir(vec2 dir);

	void set_scale(float scale);

	void scale_by(float scale);
	
	float exploding_timer;

	bool bullet_type;

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	bool m_is_alive; // True if the salmon is alive
	size_t m_num_indices; // passed to glDrawElements
	vec2 m_movement_dir;
	vec2 m_mouse;
};
