#pragma once

#include "common.hpp"

class Bullet : public Renderable
{
public:
	bool init(float angle, vec2 position);

	void destroy();

	void update(float ms);

	void draw(const mat3& projection)override;

	vec2 get_position()const;

	void move(vec2 off);

	void set_rotation(float radians);

	void set_movement_dir(vec2 dir);

private:
	vec2 m_position;
	vec2 m_scale;
	float m_rotation;
	vec2 m_movement_dir;
	size_t m_num_indices;
};