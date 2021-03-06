#include "Structure.hpp"
#include "Spawner.hpp"
#include "TextureManager.hpp"

#include <cmath>

std::vector<PlayerBullet>* Structure::player_bullets;
std::vector<EnemyBullet>* Structure::enemy_bullets;
Player* Structure::player;

Structure::Structure() : Sprite(nullptr) {
	health = 5;
}

void Healing_Structure::update(float ms) {
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	heal_boss(ms);
}

void Structure::scale_by(float scale) {
	m_scale.x *= scale;
	m_scale.y *= scale;
}

void Healing_Structure::heal_boss(float ms) {
	b->set_health(ms/1000.f*5.f);
	//printf("Boss health: %f\n", b->get_health());
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
	// vec2 position, vec2 movement_dir, Texture* texture,
	TextureManager* tm = TextureManager::get_instance();
	spawn_enemy_bullet(m_position, player->get_position() - m_position, tm->get_texture("enemy0"), enemy_bullets);
}

bool Black_Hole_Structure::init() {
	time_until_expiry = 3000;
	return Structure::init();
}

void Black_Hole_Structure::update(float ms) {
	//printf("update blackhole\n");
	//time_until_expiry -= ms;
	if (time_until_expiry <= 0) {
		return;
	}
	float mass = 100.f;
	float bullet_mass = 1.f;
	float numerator = GRAVITY_CONST * mass * bullet_mass;
	force b_g_force;
	//auto little_enemy_it = m_little_enemies.begin(); little_enemy_it != m_little_enemies.end();
	for (auto bullet_it = (*player_bullets).begin(); bullet_it != (*player_bullets).end();) {
		//printf("these are the player bullets");
		vec2 diff = m_position - bullet_it->get_position();
		b_g_force.dir = normalize(diff);
	
		b_g_force.mag = numerator / std::pow(length(diff), 2.0f);
		bullet_it->addForce(b_g_force);
		//printf("Force bullet: %f\n", b_g_force.dir.y);
		bullet_it++;
	}
}