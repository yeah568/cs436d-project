#include "Level.hpp"

bool CustomMenu::init() {
	m_background.set_texture(tm->get_texture("mainmenu"));
	local_button.set_texture(tm->get_texture("button"));
	back_button.set_texture(tm->get_texture("button"));
	download_button.set_texture(tm->get_texture("button"));

	m_background.init();
	local_button.init("Exo2-Light.ttf", "Local");
	back_button.init("Exo2-Light.ttf", "Back");
	download_button.init("Exo2-Light.ttf", "Download");
//
	local_button.set_scale({1.f,1.f});
	local_button.set_rotation(0);
	back_button.set_scale({1.f,1.f});
	back_button.set_rotation(0);
	download_button.set_rotation(0);
	download_button.set_scale({1.f,1.f});

	local_button.set_position({screen.x/4.f, screen.y/2.f});

	download_button.set_position({ screen.x/4.f*3.f, screen.y/2.f });

	back_button.set_position({ 200.f, 850.f});
	bbox temp = back_button.get_bounding_box();
	float width = temp.max_x - temp.min_x;
	float height = temp.max_y - temp.min_y;
	back_button.set_position({ width/2.f, screen.y-height/2.f});

	m_background.set_scale({1.f,1.f});
	m_background.set_rotation(0);
	m_background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });

	temp = m_background.get_bounding_box();
	width = temp.max_x - temp.min_x;
	height = temp.max_y - temp.min_y;
	m_background.set_scale({screen.x/width,screen.y/height});

	return 1;
	
}
void CustomMenu::draw()
{
	int w = screen.x;
	int h = screen.y;
	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5;
	float top = 0.f;// (float)h * -0.5;
	float right = (float)w;// *0.5;
	float bottom = (float)h;// *0.5;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	m_background.draw(projection_2D);
	back_button.draw(projection_2D);
	local_button.draw(projection_2D);
	download_button.draw(projection_2D);
	
}

void CustomMenu::destroy() { 
	m_background.destroy();
	back_button.destroy();
	local_button.destroy();
	download_button.destroy();
}

bool CustomMenu::update(float ms) { return 1; };


void CustomMenu::on_mouse_click(vec2 pos) {
	if (local_button.was_clicked(pos)) {
		finished = 1;
		levelList->emplace_back(new CustomLocalMenu(screen.x, screen.y));
	}
	else if (back_button.was_clicked(pos)) {
		finished = 1;
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
	}
	else if (download_button.was_clicked(pos)) {
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
		finished = 1;
	}

}

void CustomMenu::on_mouse_move(double xpos, double ypos) {
	vec2 pos = {(float)xpos, (float)ypos};
	local_button.set_text_color({1,1,1});
	back_button.set_text_color({1,1,1});
	download_button.set_text_color({1,1,1});
	if (local_button.was_clicked(pos)) {
		local_button.set_text_color({0,1,0});
	} else if (back_button.was_clicked(pos)) {
		back_button.set_text_color({0,1,0});
	} else if (download_button.was_clicked(pos)) {
		download_button.set_text_color({0,1,0});
	}
}
