#include "Structure.hpp"

#include <cmath>


Structure::Structure() : Sprite(nullptr) {}

void Healing_Structure::update(float ms) {
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	heal_boss(ms);
}

bool Structure::collides_with(const Bullet &bullet) {
	bbox enemy_bbox = get_bounding_box();
	bbox bullet_bbox = bullet.get_bounding_box();
	return bullet_bbox.min_x <= enemy_bbox.max_x && bullet_bbox.max_x >= enemy_bbox.min_x &&
		bullet_bbox.min_y <= enemy_bbox.max_y && bullet_bbox.max_y >= enemy_bbox.min_y;
}

void Structure::scale_by(float scale) {
	m_scale.x *= scale;
	m_scale.y *= scale;
}

void Healing_Structure::heal_boss(float ms) {
	b->set_health(ms/1000*20);
	printf("Boss health: %f\n", b->get_health());
}

void Structure::update(float ms) {}
