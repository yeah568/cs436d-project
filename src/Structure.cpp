#include "Structure.hpp"

#include <cmath>

std::vector<Bullet>* Structure::player_bullets;

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
	b->set_health(ms/1000.f*20.f);
	printf("Boss health: %f\n", b->get_health());
}

void Structure::update(float ms) {}

void Shooting_Structure::update(float ms) {
	time_until_shoot -= ms;
	if (time_until_shoot <= 0) {
		shoot();
		time_until_shoot = 2000;
	}
}

void Shooting_Structure::shoot() {
	// TODO:
}

void Black_Hole_Structure::update(float ms) {
	time_until_expiry -= ms;
	if (time_until_expiry <= 0) {
		return;
	}
	float mass = 10000000000.f;
	float bullet_mass = 1000.f;
	float numerator = GRAVITY_CONST * mass * bullet_mass;
	force b_g_force;
	for (auto bullet : *player_bullets) {
		b_g_force.dir = normalize(m_position - bullet.get_position());
		b_g_force.mag = numerator / std::pow(length(m_position - bullet.get_position()), 2.0);
		bullet.addForce(b_g_force);
	}
}