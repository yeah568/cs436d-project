#include "Level.hpp"
#include <fstream>
#include <iostream>


bool EndScreen::init() {
	background.set_texture(tm->get_texture("mainmenu"));
	exit_button.set_texture(tm->get_texture("button"));
	input_button.set_texture(tm->get_texture("button"));

	background.init();

	exit_button.init("Exo2-Light.ttf", "Back");
	input_button.init("Exo2-Light.ttf", " ");

	//
	exit_button.set_scale({ 1.f,1.f });
	exit_button.set_rotation(0);

	exit_button.set_position({ 200.f, 850.f });

	input_button.set_scale({ 1.f,1.f });
	input_button.set_rotation(0);

	input_button.set_position({ 400.f, 850.f });


	background.set_scale({ 1.f,1.f });
	background.set_rotation(0);
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });


	bbox temp = background.get_bounding_box();
	float bg_width = temp.max_x - temp.min_x;
	float bg_height = temp.max_y - temp.min_y;
	background.set_scale({ screen.x / bg_width,screen.y / bg_height });

	Button* entry = nullptr;
	float sb_width = 0;
	float sb_height = 0;

	return 1;

}
void EndScreen::draw()
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

	bbox bb;


	background.draw(projection_2D);
	exit_button.draw(projection_2D);
	input_button.draw(projection_2D);

}

void EndScreen::destroy() {
	background.destroy();
	exit_button.destroy();
	input_button.destroy();
}

bool EndScreen::update(float ms) { return 1; };


void EndScreen::on_mouse_click(vec2 pos) {
	if (exit_button.was_clicked(pos)) {
		// read in leaderboard text file
		// append text in the button + their score to whatever holds leaderboard text
		// sort
		// write it out
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
		finished = 1;
	}

}

void EndScreen::on_mouse_move(double xpos, double ypos) {
	vec2 pos = { (float)xpos, (float)ypos };
	exit_button.set_text_color({ 1,1,1 });
	if (exit_button.was_clicked(pos)) {
		exit_button.set_text_color({ 0,1,0 });
	}
}

void EndScreen::on_key(int key, int action, int mod) {
	char temp;
	// Letters
	if (key >= 65 && key <= 90 && action == GLFW_PRESS) {
		temp = (char)key;
		input_button.set_text(input_button.get_text() + temp);
	}
	// Numbers
	else if (key >= 48 && key <= 57 && action == GLFW_PRESS) {
		temp = (char)key;
		input_button.set_text(input_button.get_text() + temp);
	}
	else if (key == 259 && action == GLFW_PRESS) {
		input_button.set_text(input_button.get_text().substr(0, input_button.get_text().size() - 1));
	}
	else if (key == 257 && action == GLFW_PRESS) {
		std::string c = input_button.get_text();
		
		std::ofstream outfile;
		outfile.open("leaderboard.txt", std::ios::app);
		outfile << c;
		outfile << std::endl;
		outfile.close();
		
	}
	else {
		return;
	}
	
}
