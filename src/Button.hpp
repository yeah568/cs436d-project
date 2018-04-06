#pragma once

#include "Sprite.hpp"
#include "TextRenderer.hpp"

class Button : public Sprite {
public:
Button() : Sprite(nullptr) {};
TextRenderer* text_r;
virtual bool init(std::string font_name, std::string str_text);
bool was_clicked(vec2 pos);
virtual void set_position(vec2 pos);
void set_text_color(vec3 col);
virtual void draw(const mat3& projection);
void set_text_alignment(int align) {alignment = align;};
void set_text(std::string temp) { text = temp; };
std::string get_text() {return text;};
virtual void set_scale(vec2 scale);
protected:
    std::string text;
    int alignment;
};

class SongButton : public Button {
public:
SongButton() : Button(), song_art(nullptr) {};
void set_position(vec2 pos);
void draw(const mat3& projection);
bool init(std::string font_name, std::string str_text);
void set_song_art_texture(Texture* tex) {song_art.set_texture(tex);};
void set_scale(vec2 scale);
private:
    Sprite song_art;
};

class LeaderboardEntry : public Button {
public:
    LeaderboardEntry() : Button() {};
    int score;
};