#include "Level.hpp"

//std::vector<Level*>* Level::levelList;

bool SongSelect::init() {
	background.set_texture(tm->get_texture("mainmenu"));
	back_button.set_texture(tm->get_texture("button"));

	background.init();
	back_button.init(EXO_FONT,"Back");

	SongButton* temp = nullptr;
	// Background, song art, song name
	std::vector<std::tuple<Texture*,Texture*,std::string>> temp2;
	temp2.emplace_back(std::make_tuple(tm->get_texture("song_box"),tm->get_texture("blends_songart"),"Blend S"));
	temp2.emplace_back(std::make_tuple(tm->get_texture("song_box"),tm->get_texture("marblesoda_songart"),"Marble Soda"));
    temp2.emplace_back(std::make_tuple(tm->get_texture("song_box"),tm->get_texture("pokemon_songart"),"Pokemon Theme"));
	float sb_width = 0;
	float sb_height = 0;
	for (int i=0; i<3; i++) {
		temp = new SongButton();
		temp->set_texture(std::get<0>(temp2[i]));
		temp->set_song_art_texture(std::get<1>(temp2[i]));
		temp->init(EXO_FONT, std::get<2>(temp2[i]));
		temp->set_scale({1,1});
		temp->set_rotation(0);
		temp->set_position({0, 0});
		sb_width = temp->get_bounding_box().max_x - temp->get_bounding_box().min_x;
		sb_height = temp->get_bounding_box().max_y - temp->get_bounding_box().min_y;
        temp->set_position({screen.x-16-sb_width/2.f, i*sb_height + sb_height/2.f});
		song_boxes.emplace_back(temp);
	}

	back_button.set_position({ 200.f, 850.f});

	background.set_scale({1,1});
	background.set_rotation(0);
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });

	bbox temp3 = background.get_bounding_box();
	float bg_width = temp3.max_x - temp3.min_x;
	float bg_height = temp3.max_y - temp3.min_y;
	background.set_scale({screen.x/bg_width,screen.y/bg_height});

	back_button.set_scale({1,1});
	back_button.set_rotation(0);
	
	return 1;
	
}
void SongSelect::draw()
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
    bbox temp;
	for (auto& btn : song_boxes) {
        temp = btn->get_bounding_box();
        if (temp.max_y > 0 && temp.min_y < h)
            btn->draw(projection_2D);
    }
	back_button.draw(projection_2D);
	
}

void SongSelect::destroy() { 
	background.destroy();
	for (auto& btn : song_boxes)
		btn->destroy();
	back_button.destroy();
}

bool SongSelect::update(float ms) { return 1; };


void SongSelect::on_mouse_click(vec2 pos) {
	for (auto& l_btn : song_boxes)
		if (l_btn->was_clicked(pos)) {
			if (l_btn->get_text() == "Blend S") {
				finished = 1;
				levelList->emplace_back(new Level1(screen.x, screen.y));
			} else if (l_btn->get_text() == "Pokemon Theme") {
				finished = 1;
				levelList->emplace_back(new Level3(screen.x, screen.y));
			} else if (l_btn->get_text() == "Marble Soda") {
				finished = 1;
				levelList->emplace_back(new Level2(screen.x, screen.y));
			}
			levelList->emplace_back(new SongSelect(screen.x, screen.y));
		}

	if (back_button.was_clicked(pos)) {
		finished = 1;
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
	}
}

void SongSelect::on_mouse_scroll(GLFWwindow* window, vec2 offset) {
    vec2 temp;
	float percentage_distance_away;
    for (auto& l_btn : song_boxes) {
        temp = l_btn->get_position();
        l_btn->set_position({temp.x, temp.y+offset.y});
//		temp = l_btn->get_position();
//		percentage_distance_away = abs(temp.y - screen.y/2)/screen.y/2;
//		l_btn->set_scale({(float)abs(cos()),(float)abs(sin(temp.y/screen.y/2*3.1415/2))});
    }
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	on_mouse_move(xpos, ypos);
}

void SongSelect::on_mouse_move(double xpos, double ypos) {
	vec2 pos = {(float)xpos, (float)ypos};
	for (auto& l_btn : song_boxes) {
		l_btn->set_text_color({1.f,1.f,1.f});
	}
	back_button.set_text_color({1.f,1.f,1.f});
	for (auto& l_btn : song_boxes)
		if (l_btn->was_clicked(pos))
			l_btn->set_text_color({0,1.f,0});

	if (back_button.was_clicked(pos)) {
		back_button.set_text_color({0,1.f,0});
	}
}