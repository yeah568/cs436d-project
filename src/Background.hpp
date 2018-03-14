#pragma once

#include "common.hpp"

// Salmon enemy 
class Background : public Renderable
{
	// Shared between all turtles, no need to load one for each instance
	

public:

	Texture background_texture;
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Update turtle due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	void set_position(vec2 pos);


private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};

