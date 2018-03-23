#include "Button.hpp"


bool Button::was_clicked(vec2 pos) {
    bbox temp = get_bounding_box();
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

void SongButton::set_position(vec2 pos) {
    Sprite::set_position(pos);
    float button_width = get_bounding_box().max_x - get_bounding_box().min_x;
    float songart_width = song_art.get_bounding_box().max_x - song_art.get_bounding_box().min_x;
    vec2 temp = {-button_width/2.f+16.f+songart_width/2.f, 0};
    song_art.set_position(pos+temp);
    text_r->setPosition(pos);
    // if (alignment==LEFT_ALIGN)
    //     text_r->setPosition(pos);
    // else {
    //     float width = text_r->get_width_of_string(text);
    //     if (alignment==CENTER_ALIGN)
    //         width *= 0.5;
    //     vec2 temp = {width,0};
    //     text_r->setPosition(pos - temp);
    // }
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

bool SongButton::init(std::string font_name, std::string str_text) {
    Button::init(font_name, str_text);
    song_art.set_scale({1.f,1.f});
    song_art.set_rotation(0);
    return song_art.init();
}

void Button::draw(const mat3& projection) {
    Sprite::draw(projection);
    text_r->renderString(projection, text);
}

void SongButton::draw(const mat3& projection) {
    Button::draw(projection);
    song_art.draw(projection);
}

void Button::set_scale(vec2 scale) {
    Sprite::set_scale(scale);
    vec2 temp = scale;
    temp.y *= -1;
    text_r->setScale(temp);
}

void SongButton::set_scale(vec2 scale) {
    Button::set_scale(scale);
    song_art.set_scale(scale);
}