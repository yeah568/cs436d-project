#pragma once

#include "common.hpp"
#include "Sprite.hpp"

// Salmon enemy 
class Background : public Sprite
{
public:
	Background() : Sprite(nullptr) {};
private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};

