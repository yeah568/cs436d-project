#pragma once

#include "Sprite.hpp"
#include "common.hpp"

class SpriteSheet : public Sprite {
public:
	SpriteSheet();
	bool init(int pictures);
	void update(float ms);
private:
	int m_pictures;
	float time_until_update;
	TexturedVertex vertices[4];
};