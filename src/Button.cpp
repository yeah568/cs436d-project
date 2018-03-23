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
    if (alignment==LEFT_ALIGN)
        text_r->setPosition(pos);
    else {
        float width = text_r->get_width_of_string(text);
        if (alignment==CENTER_ALIGN)
            width *= 0.5;
        vec2 temp = {width,0};
        text_r->setPosition(pos - temp);
    }
}

void Button::set_text_color(vec3 col) {
    text_r->setColour(col);
}

bool Button::init(std::string font_name, std::string str_text) {
    text_r = new TextRenderer(font_name, 48);
    text_r->setColour({1.f,1.f,1.f});
    text = str_text;
    alignment = CENTER_ALIGN;
    return Sprite::init();
}

void Button::draw(const mat3& projection) {
    Sprite::draw(projection);
    text_r->renderString(projection, text);
}