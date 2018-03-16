#include "Button.hpp"


bool Button::was_clicked(vec2 pos) {
    bbox temp = get_bounding_box();
    return temp.max_x <= pos.x && temp.min_x >= pos.x && temp.max_y <= pos.y && temp.min_y >= pos.y;
}