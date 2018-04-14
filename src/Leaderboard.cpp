#include "Level.hpp"
#include <fstream>
#include <iostream>


struct S {
	char a[256];
};

bool sortByScore(const LeaderboardEntry* lhs, const LeaderboardEntry* rhs) { return lhs->score > rhs->score; }

bool Leaderboard::init() {
	std::ifstream inFile;
	inFile.open(LEADERBOARD_PATH.c_str());
	if (!inFile) {
		printf("issues");
	}
	char name[256];
	std::vector<S> names;
	S s;
	while (inFile.getline(s.a, 256)) {
		names.emplace_back(s);
	}
	inFile.close();

	title = new TextRenderer("Exo2-Light.ttf", 48);
	int title_width = title->get_width_of_string("Leaderboard");
	title->setPosition({screen.x/33.f, 50});
	/*
	std::ofstream outfile;
	outfile.open("leaderboard.txt", std::ios::trunc);
	outfile << "This is a line \n";
	outfile << "This is also a line\n";
	outfile.close();
	*/
	background.set_texture(tm->get_texture("mainmenu"));
	exit_button.set_texture(tm->get_texture("button"));

	background.init();

	exit_button.init("Exo2-Light.ttf", "Back");

	//
	exit_button.set_scale({ 1.f,1.f });
	exit_button.set_rotation(0);

	exit_button.set_position({ 200.f, 850.f });

	background.set_scale({ 1.f,1.f });
	background.set_rotation(0);
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });

	
	bbox temp = background.get_bounding_box();
	float bg_width = temp.max_x - temp.min_x;
	float bg_height = temp.max_y - temp.min_y;
	background.set_scale({ screen.x / bg_width,screen.y / bg_height });

	LeaderboardEntry* entry = nullptr;
	float sb_width = 0;
	float sb_height = 0;
	for (int i = 0; i<names.size(); i++) {
		entry = new LeaderboardEntry();
		entry->set_texture(tm->get_texture("button"));
		std::string file_text(names[i].a);
		entry->score = parse_score(file_text);
		entry->init(EXO_FONT, names[i].a);
		entry->set_scale({ 1,1 });
		entry->set_rotation(0);
		entry->set_position({ 0, 0 });
		sb_width = entry->get_bounding_box().max_x - entry->get_bounding_box().min_x;
		sb_height = entry->get_bounding_box().max_y - entry->get_bounding_box().min_y;
		entry->set_position({ screen.x - 16 - sb_width / 2.f, i*sb_height + sb_height / 2.f });
		entries.emplace_back(entry);
	}
	sort(entries.begin(), entries.end(), sortByScore);
	for (int j = 0; j<entries.size(); j++) {
		entry = entries[j];
		sb_width = entry->get_bounding_box().max_x - entry->get_bounding_box().min_x;
		sb_height = entry->get_bounding_box().max_y - entry->get_bounding_box().min_y;
		entry->set_position({ screen.x - 16 - sb_width / 2.f, j*sb_height + sb_height / 2.f });
	}
	return 1;

}

int Leaderboard::parse_score(std::string parsable) {
	int result=0;
	std::string str2("||");
	std::size_t found = parsable.find(str2);
	if (found!=std::string::npos) {
		std::string str3 = parsable.substr(found+2,parsable.length()-found-2);
		result = atoi(str3.c_str());
	}

	return result;
}
void Leaderboard::draw()
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
	for (auto& btn : entries) {
		bb = btn->get_bounding_box();
		if (bb.max_y > 0 && bb.min_y < h)

			btn->draw(projection_2D);
	}
	title->renderString(projection_2D, "Leaderboard");

}

void Leaderboard::destroy() {
	exit_button.destroy();
	songselect_button.destroy();
	for (auto& btn : entries)
		btn->destroy();
}

bool Leaderboard::update(float ms) { return 1; };


void Leaderboard::on_mouse_click(vec2 pos) {
	if (exit_button.was_clicked(pos)) {
		levelList->emplace_back(new MainMenu(screen.x, screen.y));
		finished = 1;
	}
		
}

void Leaderboard::on_mouse_move(double xpos, double ypos) {
	vec2 pos = { (float)xpos, (float)ypos };
	exit_button.set_text_color({ 1,1,1 });
	if (exit_button.was_clicked(pos)) {
		exit_button.set_text_color({ 0,1,0 });
	}
}


void Leaderboard::on_mouse_scroll(GLFWwindow* window, vec2 offset){
	vec2 temp;
	for (auto lbe : entries) {
		temp = lbe->get_position();
		lbe->set_position({temp.x,temp.y+offset.y});
	}
}