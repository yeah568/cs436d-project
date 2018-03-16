#include "Level.hpp"


bool MainMenu::init() {
	background.set_texture(tm->get_texture("character"));
	background.init(50, 100);
	background.set_position({ (float)screen.x / 2, (float)screen.y / 2 });
	
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
	
}

void MainMenu::destroy() { background.destroy(); };
bool MainMenu::update(float ms) { return 1; };

