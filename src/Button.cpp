#include "Button.hpp"


bool Button::was_clicked(vec2 pos) {
    bbox temp = get_bounding_box();
    printf("Click X: %f, Click Y: %f\n", pos.x, pos.y);
    printf("Button X: %f, Button Y: %f\n", temp.min_x, temp.min_y);
    vec2 diff = {(temp.max_x - temp.min_x)/2.f, (temp.max_y - temp.min_y)/2.f};
    return pos.x <= diff.x + m_position.x && pos.y <= diff.y + m_position.y && pos.x >= m_position.x - diff.x && pos.y >= m_position.y - diff.y;
}

void Button::set_position(vec2 pos) {
    Sprite::set_position(pos);
    text_r->setPosition(pos);
}

void Button::set_text_color(vec3 col) {
    text_r->setColour(col);
}

bool Button::init(std::string font_name, std::string str_text) {
    text_r = new TextRenderer(font_name, 25);
    text_r->setColour({1.f,1.f,1.f});
    text = str_text;
    return Sprite::init();
}

void Button::draw(const mat3& projection) {
    Sprite::draw(projection);
    text_r->setPosition(m_position);
    text_r->setColour({0.8f,0.8f,0.8f});
    text_r->renderString(projection, "text");
    printf("Finished rendering text\n");
}