#pragma once

#include "common.hpp"

class Hitbox : public Renderable
{

public:
	Hitbox();
	// Creates all the associated render resources and default transform
	bool init(vec2 tr, vec2 sc, float rot, Renderable* parent, Texture* parent_texture);
	void destroy();

	void draw(const mat3& projection) override;

	mat3 get_matrix()const;

	void set_matrix(mat3 matrix);

protected:
	static Texture* m_texture;
	static void load_hitbox_texture();
	mat3 m_matrix;
	Renderable* m_parent;
	float m_color[3];
};