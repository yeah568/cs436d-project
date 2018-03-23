#pragma once

#include "Sprite.hpp"
#include "TextRenderer.hpp"

class Button : public Sprite {
public:
Button() : Sprite(nullptr) {};
TextRenderer* text_r;
bool init(std::string font_name, std::string str_text);
bool was_clicked(vec2 pos);
void set_position(vec2 pos);
void set_text_color(vec3 col);
void draw(const mat3& projection);
void set_text_alignment(int align) {alignment = align;};
private:
    std::string text;
    int alignment;
};