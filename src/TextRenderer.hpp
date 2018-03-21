#pragma once

#include "common.hpp"

#include <string.h>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H  


struct Character {
	GLuint textureID;  // ID handle of the glyph texture
	vec2 size;       // Size of glyph
	vec2 bearing;    // Offset from baseline to left/top of glyph
	GLuint advance;    // Offset to advance to next glyph
};


class TextRenderer : Renderable
{
public:
	TextRenderer(std::string font_name);
	TextRenderer(std::string font_name, vec3 colour);
	void setColour(vec3 colour);
	void setPosition(vec2 position);
	void setScale(vec2 scale);
	void renderString(const mat3& projection, std::string text);
	void draw(const mat3& projection);
private:
	std::map<GLchar, Character> characters;
	vec3 m_colour;

	GLuint vao;
	GLuint vbo;

	vec2 m_position;
	vec2 m_scale;
};
