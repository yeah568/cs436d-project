#include "Button.hpp"


bool Button::was_clicked(vec2 pos) {
    bbox temp = get_bounding_box();
    printf("Click X: %f, Click Y: %f\n", pos.x, pos.y);
    printf("Button X: %f, Button Y: %f\n", temp.min_x, temp.min_y);
    vec2 diff = {(temp.max_x - temp.min_x)/2.f, (temp.max_y - temp.min_y)/2.f};
    return pos.x <= diff.x + m_position.x && pos.y <= diff.y + m_position.y && pos.x >= m_position.x - diff.x && pos.y >= m_position.y - diff.y;
}