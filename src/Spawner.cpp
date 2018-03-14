#include "Spawner.hpp"
#include "Bullet.hpp"
#include <math.h>

void spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<PlayerBullet>> bullets) {
    std::shared_ptr<PlayerBullet> bullet = spawn_player_bullet(position, angle, scale, dmg, spd, texture);
    if (bullet != nullptr)
        bullets->emplace_back(*bullet);
}

std::shared_ptr<PlayerBullet> spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, std::shared_ptr<Texture> texture) {
    std::shared_ptr<PlayerBullet> bullet = std::make_shared<PlayerBullet>();
	bullet->set_texture(texture);
	if (bullet->init(dmg, spd))
	{
		
		bullet->set_position(position);
		bullet->set_rotation(angle);
		bullet->set_scale(scale);
		bullet->m_movement_dir = { (float)cos(angle), (float)-sin(angle) };

		return bullet;
	}
	fprintf(stderr, "Failed to spawn fish");
	return nullptr;
}

void spawn_enemy_bullet(vec2 position, vec2 movement_dir, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<EnemyBullet>> bullets) {
    std::shared_ptr<EnemyBullet> bullet = spawn_enemy_bullet(position, movement_dir, texture);
    if (bullet != nullptr)
        bullets->emplace_back(*bullet);
}

std::shared_ptr<EnemyBullet> spawn_enemy_bullet(vec2 position, vec2 movement_dir, std::shared_ptr<Texture> texture) {
    // TODO:
    std::shared_ptr<EnemyBullet> bullet = std::make_shared<EnemyBullet>();
    bullet->set_texture(texture);
	if (bullet->init(2.f, 200.f))
	{
		
		bullet->set_position(position);
		bullet->set_rotation(1.57f+atan(movement_dir.y/movement_dir.x));
		bullet->m_movement_dir = normalize(movement_dir);

		return bullet;
	}
	fprintf(stderr, "Failed to spawn fish");
	return nullptr;
}

std::shared_ptr<LittleEnemy> spawn_little_enemy(vec2 position, std::shared_ptr<Texture> texture) {
    std::shared_ptr<LittleEnemy> littleEnemy = std::make_shared<LittleEnemy>();
	littleEnemy->set_texture(texture);
	if (littleEnemy->init()) {
        littleEnemy->set_position(position);
        return littleEnemy;
    }
	fprintf(stderr, "Failed to spawn little enemy");
	return nullptr;
}

void spawn_little_enemy(vec2 position, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<LittleEnemy>> enemies) {
    std::shared_ptr<LittleEnemy> le = spawn_little_enemy(position, texture);
    if (le != nullptr)
        enemies->emplace_back(*le);
}

std::shared_ptr<Structure> spawn_structure(int type, std::shared_ptr<Texture> texture) {
    printf("spawning structure\n");
    std::shared_ptr<Structure> new_structure;
    vec2 scale = {1.f, 1.f};
    std::shared_ptr<Healing_Structure> temp;
    switch (type) {
        case HEALING_STRUCTURE:
            temp = std::make_shared<Healing_Structure>();
            new_structure = temp;
            break;
        case BLACK_HOLE_STRUCTURE:
            new_structure = std::make_shared<Black_Hole_Structure>();
            scale = 0.5 * scale;
            break;
        case SHOOTING_STRUCTURE:
            new_structure = std::make_shared<Shooting_Structure>();
            scale = 3 * scale;
            break;
    }
    new_structure->set_texture(texture);
    if (!(new_structure->init({0,0}, scale, 0))) {
        printf("Issues\n");
        return nullptr;
    }
    printf("Finished\n");
    return new_structure;
}

void spawn_structure(int type, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<std::shared_ptr<Structure>>>structures) {
    std::shared_ptr<Structure> structure = spawn_structure(type, texture);
    if (structure != nullptr)
        structures->emplace_back(structure);
}

void spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<BeatCircle>> bcs) {
    std::shared_ptr<BeatCircle> bc = spawn_beat_circle(dir, pos, speed, scale, abs_offset, texture);
    if (bc != nullptr)
        bcs->emplace_back(*bc);
}

std::shared_ptr<BeatCircle> spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, std::shared_ptr<Texture> texture) {
    std::shared_ptr<BeatCircle> beat_circle = std::make_shared<BeatCircle>(speed, abs_offset);
    bool type = ((dir % 2) == 1);
    beat_circle->set_texture(texture);
    if (beat_circle->init()) {
        beat_circle->set_dir(dir, pos);
        beat_circle->set_scale({scale, scale});
        return beat_circle;
    }
    fprintf(stderr, "Failed to spawn beat circle");
    return nullptr;
}