#pragma once

#include "common.hpp"
#include "bullet.hpp"
#include "BeatList.hpp"
#include "enemy.hpp"

#include <vector>

class Boss : public Renderable
{

public:
	// Creates all the associated render resources and default transform
	bool init(float health, std::vector<LittleEnemy>* little_enemies);
	static Texture boss_texture;

	// Releases all associated resources
	void destroy();

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	void draw(const mat3& projection)override;

	void on_beat(Beat* beat, vec2 screen);

	bool collides_with(const Bullet& bullet);

	vec2 get_position()const;

	float get_rotation()const;

	void move(vec2 off);

	void set_rotation(float radians);

	void set_scale(vec2 scale);

	vec2 get_scale();

	float get_health()const;

	void set_health(float delta);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	float m_health;
	std::vector<LittleEnemy>* m_little_enemies;
};