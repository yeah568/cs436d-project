#include "Level.hpp"


bool MainMenu::init() {
	background.set_texture(tm->get_texture("mainmenu"));
	play_button.set_texture(tm->get_texture("play_button"));
	exit_button.set_texture(tm->get_texture("exit_button"));

	background.init();
	play_button.init();
	exit_button.init();

	bbox temp = play_button.get_bounding_box();
	play_button.set_position({(temp.min_x + temp.max_x)/2.f, (temp.min_y + temp.max_y)/2.f});
	temp = exit_button.get_bounding_box();
	exit_button.set_position({(temp.min_x + temp.max_x)/2.f, screen.y - (temp.min_y + temp.max_y)/2.f});
	background.set_scale({0.75,0.75});
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });
	background.set_rotation(0);

	play_button.set_scale({1,1});
	play_button.set_rotation(0);

	exit_button.set_scale({1,1});
	exit_button.set_rotation(0);
	
	return 1;
	
}
void MainMenu::draw()
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

	background.draw(projection_2D);
	exit_button.draw(projection_2D);
	play_button.draw(projection_2D);
	
}

void MainMenu::destroy() { 
	background.destroy();
	play_button.destroy();
	exit_button.destroy();
}

bool MainMenu::update(float ms) { return 1; };


void MainMenu::on_mouse_click(vec2 pos) {
	if (play_button.was_clicked(pos))
		finished = 1;
	else if (exit_button.was_clicked(pos))
		exit(0);
}
