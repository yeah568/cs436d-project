#include "Spawner.hpp"
#include "Bullet.hpp"
#include <math.h>

void spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, Texture* texture, std::vector<PlayerBullet>* bullets) {
    PlayerBullet* bullet = spawn_player_bullet(position, angle, scale, dmg, spd, texture);
    if (bullet != nullptr)
        bullets->emplace_back(*bullet);
}

PlayerBullet* spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, Texture* texture) {
    PlayerBullet* bullet = new PlayerBullet;
	bullet->set_texture(texture);
	if (bullet->init(dmg, spd))
	{
		
		bullet->set_position(position);
		bullet->set_rotation(angle);
		bullet->set_scale(scale);
		bullet->m_movement_dir = { -10, 20 };

		return bullet;
	}
	fprintf(stderr, "Failed to spawn fish");
	return nullptr;
}

void spawn_enemy_bullet(vec2 position, vec2 movement_dir, Texture* texture, std::vector<EnemyBullet>* bullets) {
    EnemyBullet* bullet = spawn_enemy_bullet(position, movement_dir, texture);
    if (bullet != nullptr)
        bullets->emplace_back(*bullet);
}

EnemyBullet* spawn_enemy_bullet(vec2 position, vec2 movement_dir, Texture* texture) {
    // TODO:
    EnemyBullet* bullet = new EnemyBullet();
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

LittleEnemy* spawn_little_enemy(vec2 position, Texture* texture) {
    LittleEnemy* littleEnemy = new LittleEnemy;
	littleEnemy->set_texture(texture);
	if (littleEnemy->init()) {
        littleEnemy->set_position(position);
        return littleEnemy;
    }
	fprintf(stderr, "Failed to spawn little enemy");
	return nullptr;
}

void spawn_little_enemy(vec2 position, Texture* texture, std::vector<LittleEnemy>* enemies) {
    LittleEnemy* le = spawn_little_enemy(position, texture);
    if (le != nullptr)
        enemies->emplace_back(*le);
}

Structure* spawn_structure(int type, Boss* boss, vec2 position, Texture* texture) {
    //printf("spawning structure\n");
    Structure* new_structure;
    vec2 scale = {1.f, 1.f};
    switch (type) {
        case HEALING_STRUCTURE:
            new_structure = new Healing_Structure;
			new_structure->type = 0;
            break;
        case BLACK_HOLE_STRUCTURE:
            new_structure = new Black_Hole_Structure;
			new_structure->type = 1;
            scale = 0.5 * scale;
            break;
        case SHOOTING_STRUCTURE:
            new_structure = new Shooting_Structure;
			new_structure->type = 2;
            scale =  scale;
            break;
    }
    new_structure->set_texture(texture);
    if (!(new_structure->init(position, scale, 0))) {
        printf("Structure couldnt init\n");
        return nullptr;
    }
    switch (type) {
        case HEALING_STRUCTURE:
            ((Healing_Structure*) new_structure)->set_boss(boss);
            break;
    }
    //printf("Finished\n");
    return new_structure;
}

void spawn_structure(int type, Boss* boss, vec2 position, Texture* texture, std::vector<Structure*>* structures) {
    Structure* structure = spawn_structure(type, boss, position, texture);
    if (structure != nullptr)
        structures->emplace_back(structure);
}

void spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, Player* player, Texture* texture, std::vector<BeatCircle>* bcs) {
    BeatCircle* bc = spawn_beat_circle(dir, pos, speed, scale, abs_offset, player, texture);
    if (bc != nullptr)
        bcs->emplace_back(*bc);
}

BeatCircle* spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, Player* player, Texture* texture) {
    BeatCircle* beat_circle = new BeatCircle(player, speed, abs_offset);
    beat_circle->set_texture(texture);
    if (beat_circle->init()) {
        beat_circle->set_dir(dir, pos);
        beat_circle->set_scale({scale, scale});
        return beat_circle;
    }
    fprintf(stderr, "Failed to spawn beat circle");
    return nullptr;
}