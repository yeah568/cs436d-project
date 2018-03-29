#include "Level.hpp"
#include "tinydir.h"

//std::vector<Level*>* Level::levelList;

bool CustomLocalMenu::init() {
	ms_since_last_click = 0;

	m_background.set_texture(tm->get_texture("mainmenu"));
	back_button.set_texture(tm->get_texture("button"));
	next_button.set_texture(tm->get_texture("button"));

	m_background.init();
	back_button.init(EXO_FONT,"Back");
	next_button.init(EXO_FONT,"Next");
	
	tinydir_open_sorted(&dir, ".");
	printf("Number of files: %d\n", dir.n_files);
	on_directory_change();

	back_button.set_position({ 200.f, 850.f});
	back_button.set_scale({1,1});
	back_button.set_rotation(0);

	auto temp = back_button.get_bounding_box();
	float width = temp.max_x - temp.min_x;
	float height = temp.max_y - temp.min_y;
	back_button.set_position({ width/2.f, screen.y-height/2.f});

	m_background.set_scale({1,1});
	m_background.set_rotation(0);
	m_background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });

	temp = m_background.get_bounding_box();
	width = temp.max_x - temp.min_x;
	height = temp.max_y - temp.min_y;
	m_background.set_scale({screen.x/width, screen.y/height});

	next_button.set_scale({1,1});
	next_button.set_rotation(0);
	next_button.set_position({ (float)screen.x / 2, (float)screen.y / 2 });

	temp = next_button.get_bounding_box();
	width = temp.max_x - temp.min_x;
	height = temp.max_y - temp.min_y;
	next_button.set_position({screen.x - width/2.f, screen.y - height/2.f});

	
	return 1;
	
}

void CustomLocalMenu::on_directory_change() {
	Button* btn_temp;
	int i = 0;
	bbox temp;
	float width;
	float height;
	for (auto btn : directory_contents)
		btn.destroy();
	directory_contents.clear();
	for (i = 0; i < dir.n_files; i++)
	{
		tinydir_file file;
		tinydir_readfile_n(&dir, &file, i);
		if (strcmp(file.name,".")==0 || strcmp(file.name,".DS_Store")==0 || !file.is_dir) {
			continue;
		}
		btn_temp = new Button();
		btn_temp->set_texture(tm->get_texture("button"));
		btn_temp->init(EXO_FONT, file.name);
		btn_temp->set_position({ 0,0});
		btn_temp->set_scale({1,1});
		btn_temp->set_rotation(0);
		temp = btn_temp->get_bounding_box();
		width = temp.max_x-temp.min_x;
		height = temp.max_y-temp.min_y;

		btn_temp->set_position({screen.x/4.f*2.f,  i*height + height/2.f});
		directory_contents.emplace_back(*btn_temp);
	}
}

void CustomLocalMenu::draw()
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
    bbox temp;
	for (auto btn : directory_contents) {
        temp = btn.get_bounding_box();
        if (temp.max_y > 0 && temp.min_y < h)
            btn.draw(projection_2D);
    }
	back_button.draw(projection_2D);
	next_button.draw(projection_2D);
	current_path.renderString(projection_2D, "PlaceHolder");
	
}

void CustomLocalMenu::destroy() { 
	m_background.destroy();
	for (auto btn : directory_contents)
		btn.destroy();
	back_button.destroy();
	directory_contents.clear();
	tinydir_close(&dir);
}

bool CustomLocalMenu::update(float ms) { 
	ms_since_last_click += ms;
	return 1;
}


void CustomLocalMenu::on_mouse_click(vec2 pos) {
	ms_since_last_click = 0;
	int index = 1;
	for (auto& l_btn : directory_contents) {
		if (l_btn.was_clicked(pos)) {
			// Double Click attempt
			if (l_btn.was_clicked_before && ms_since_last_click <= 1000) {
				printf("Number of files: %d\n", dir.n_files);
				tinydir_open_subdir_n(&dir, index);
				on_directory_change();
				l_btn.was_clicked_before = false;
				return;
			// Single Click
			} else {
				l_btn.set_text_color({0.f,1.f,0.f});
				l_btn.was_clicked_before = true;
			}
		} else {
			l_btn.was_clicked_before = false;
			l_btn.set_text_color({1.f,1.f,1.f});
		}
		index += 1;
	}
	if (back_button.was_clicked(pos)) {
		finished = 1;
		levelList->emplace_back(new SongSelect(screen.x, screen.y));
	}
}

void CustomLocalMenu::on_mouse_scroll(GLFWwindow* window, vec2 offset) {
    vec2 temp;
    for (auto& l_btn : directory_contents) {
        temp = l_btn.get_position();
        l_btn.set_position({temp.x, temp.y+offset.y});
//		temp = l_btn->get_position();
//		percentage_distance_away = abs(temp.y - screen.y/2)/screen.y/2;
//		l_btn->set_scale({(float)abs(cos()),(float)abs(sin(temp.y/screen.y/2*3.1415/2))});
    }
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	on_mouse_move(xpos, ypos);
}

void CustomLocalMenu::on_mouse_move(double xpos, double ypos) {
	vec2 pos = {(float)xpos, (float)ypos};
	back_button.set_text_color({1.f,1.f,1.f});
	next_button.set_text_color({1.f,1.f,1.f});

	if (back_button.was_clicked(pos)) {
		back_button.set_text_color({0,1.f,0});
	} else if (next_button.was_clicked(pos)) {
		next_button.set_text_color({0,1.f,0});
	}
}
