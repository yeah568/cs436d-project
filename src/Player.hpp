#pragma once

#ifndef Player_H
#define Player_H

#include "common.hpp"
#include "Enemy.hpp"
#include "Sprite.hpp"
#include <algorithm>

class Player : public Sprite
{
	
public:
  Player();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

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

	void set_health(float delta) {m_health += std::min(5-get_health(),delta);};

	float get_health() {return m_health;};

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	bool m_is_alive; // True if the salmon is alive
	size_t m_num_indices; // passed to glDrawElements
	vec2 m_movement_dir;
	vec2 m_mouse;
	float m_health;
};

#endif
