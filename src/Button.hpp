#pragma once

#include "Sprite.hpp"

class Button : public Sprite {
public:
Button() : Sprite(nullptr) {};

bool was_clicked(vec2 pos);
};