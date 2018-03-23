#include "Level.hpp"

//std::vector<Level*>* Level::levelList;

bool SongSelect::init() {
	background.set_texture(tm->get_texture("mainmenu"));
	back_button.set_texture(tm->get_texture("back_button"));

	background.init();
	back_button.init(EXO_FONT,"Back");

	Button* temp = nullptr;
	std::vector<std::pair<Texture*,std::string>> temp2;
	temp2.emplace_back(std::make_pair(tm->get_texture("blends_song_box"),"Blend S"));
	temp2.emplace_back(std::make_pair(tm->get_texture("marblesoda_song_box"),"Marble Soda"));
    temp2.emplace_back(std::make_pair(tm->get_texture("pokemon_song_box"),"Pokemon Theme"));
	for (int i=0; i<2; i++) {
		temp = new Button();
		temp->set_texture(std::get<0>(temp2[i]));
		temp->init(EXO_FONT, std::get<1>(temp2[i]));
        temp->set_position({500.f, i*250.f + 250});
        temp->set_scale({1,1});
        temp->set_rotation(0);
		song_boxes.emplace_back(temp);
	}

	back_button.set_position({ 200.f, 850.f});
	background.set_scale({0.75,0.75});
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });
	background.set_rotation(0);

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
    vec2 temp;
	for (auto& btn : song_boxes) {
        temp = btn->get_position();
        if (temp.y > 0 && temp.y < h)
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
		if (l_btn->was_clicked(pos))
			finished = 1;
	if (back_button.was_clicked(pos)) {
		finished = 1;
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
	}
}

void SongSelect::on_mouse_scroll(vec2 offset) {
    vec2 temp;
    for (auto& l_btn : song_boxes) {
        temp = l_btn->get_position();
        l_btn->set_position({temp.x, temp.y+offset.y});
    }
}