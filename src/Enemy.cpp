#include "Enemy.hpp"

#include <cmath>

Player* LittleEnemy::player;

LittleEnemy::LittleEnemy() : Sprite(nullptr) {
	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = -0.4f;
	m_scale.y = 0.4f;
	m_rotation = 0.f;
}

void LittleEnemy::update(float ms) {
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	const float enemy_speed = 100.f;
	float step = enemy_speed * (ms / 1000);
	vec2 to_player = normalize(player->get_position() - get_position());
	m_position.y += to_player.y*step;
	m_position.x += to_player.x*step;
	vec2 player_pos = player->get_position();
	float delta_x = player_pos.x - m_position.x;
	float delta_y = m_position.y - player_pos.y;
	float angle = (float)atan2(delta_y, delta_x);
	m_rotation = angle + 3.14f/2;
	//m_position.x += step;

	//
	//        vec2 normalized_movement = m_movement_dir;
	//        if (m_movement_dir.x != 0 && m_movement_dir.y != 0) {
	//            normalized_movement = normalize(normalized_movement);
	//        }
	//
	//        move({ normalized_movement.x * 10, normalized_movement.y * 10 });
	//
	//        // Set player to face mouse
	//    //TODO set the particles to move toward player
	//        float delta_x = m_mouse.x - m_position.x;
	//        float delta_y = m_position.y - m_mouse.y;
	//        float angle = (float)atan2(delta_y, delta_x);
	//        set_rotation(3.14/2);
	//
	//    printf("updated enemy");
}

void LittleEnemy::scale_by(float scale) {
	m_scale.x *= scale;
	m_scale.y *= scale;
}

