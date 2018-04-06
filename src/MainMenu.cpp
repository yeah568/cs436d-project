#include "Level.hpp"


std::vector<Level*>* Level::levelList;

bool MainMenu::init() {
	background.set_texture(tm->get_texture("mainmenu"));
	play_button.set_texture(tm->get_texture("button"));
	exit_button.set_texture(tm->get_texture("button"));
	songselect_button.set_texture(tm->get_texture("button"));
	leaderboard.set_texture(tm->get_texture("button"));

	background.init();
	play_button.init("Exo2-Light.ttf", "Play");
	exit_button.init("Exo2-Light.ttf", "Exit");
	songselect_button.init("Exo2-Light.ttf", "Song Select");
	leaderboard.init("Exo2-Light.ttf", "Leaderboard");
//
	exit_button.set_scale({1.f,1.f});
	exit_button.set_rotation(0);
	play_button.set_scale({1.f,1.f});
	play_button.set_rotation(0);
	songselect_button.set_rotation(0);
	songselect_button.set_scale({1.f,1.f});
	leaderboard.set_scale({ 1.f,1.f });
	leaderboard.set_rotation(0);

	play_button.set_position({200.f, 200.f});

	songselect_button.set_position({ 200.f, (float)screen.y / 2 });

	exit_button.set_position({ 200.f, 850.f});

	leaderboard.set_position({ 200.f, 600});

	background.set_scale({1.f,1.f});
	background.set_rotation(0);
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });

	bbox temp = background.get_bounding_box();
	float bg_width = temp.max_x - temp.min_x;
	float bg_height = temp.max_y - temp.min_y;
	background.set_scale({screen.x/bg_width,screen.y/bg_height});

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
	songselect_button.draw(projection_2D);
	leaderboard.draw(projection_2D);
	
}

void MainMenu::destroy() { 
	background.destroy();
	play_button.destroy();
	exit_button.destroy();
	songselect_button.destroy();
	leaderboard.destroy();
}

bool MainMenu::update(float ms) { return 1; };


void MainMenu::on_mouse_click(vec2 pos) {
	if (play_button.was_clicked(pos)) {
		finished = 1;
		levelList->emplace_back(new Level1(screen.x, screen.y));
		levelList->emplace_back(new Level2(screen.x, screen.y));
        levelList->emplace_back(new Level3(screen.x, screen.y));
		levelList->emplace_back(new EndScreen(screen.x, screen.y));
		levelList->emplace_back(new Leaderboard(screen.x, screen.y));
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
	}
	else if (exit_button.was_clicked(pos))
		exit(0);
	else if (songselect_button.was_clicked(pos)) {
		levelList->emplace_back(new SongSelect(screen.x, screen.y));
		finished = 1;
	}
	else if (leaderboard.was_clicked(pos)) {
		levelList->emplace_back(new Leaderboard(screen.x, screen.y));
		finished = 1;
	}

}

void MainMenu::on_mouse_move(double xpos, double ypos) {
	vec2 pos = {(float)xpos, (float)ypos};
	play_button.set_text_color({1,1,1});
	exit_button.set_text_color({1,1,1});
	leaderboard.set_text_color({ 1,1,1 });
	songselect_button.set_text_color({1,1,1});
	if (play_button.was_clicked(pos)) {
		play_button.set_text_color({0,1,0});
	} else if (exit_button.was_clicked(pos)) {
		exit_button.set_text_color({0,1,0});
	} else if (songselect_button.was_clicked(pos)) {
		songselect_button.set_text_color({0,1,0});
	}
	else if (leaderboard.was_clicked(pos)) {
		leaderboard.set_text_color({ 0,1,0 });
	}
}
