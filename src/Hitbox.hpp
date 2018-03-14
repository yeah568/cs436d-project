#pragma once

#ifndef Hitbox_H
#define Hitbox_H

#include "common.hpp"
#include "Sprite.hpp"

class Sprite;

class Hitbox : public Renderable
{

public:
	Hitbox(Renderable* p);
	// Creates all the associated render resources and default transform
	bool init(vec2 tr, vec2 sc, float rot, std::shared_ptr<Texture> parent_texture);
	void destroy();

	void draw(const mat3& projection) override;

	mat3 get_matrix()const;

	void set_matrix(mat3 matrix);

protected:
	static std::shared_ptr<Texture> m_texture;
	static void load_hitbox_texture();
	mat3 m_matrix;
	std::shared_ptr<Sprite> m_parent;
	float m_color[3];
};

#endif
