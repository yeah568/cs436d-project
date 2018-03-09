#include "Spawner.hpp"
#include "Bullet.hpp"
#include <math.h>

void spawn_player_bullet(vec2 position, bool on_beat, Texture* texture, std::vector<PlayerBullet>* bullets) {
    PlayerBullet* bullet = spawn_player_bullet(position, on_beat, texture);
    if (bullet != nullptr)
        bullets->emplace_back(*bullet);
}

PlayerBullet* spawn_player_bullet(vec2 position, bool on_beat, Texture* texture) {
    PlayerBullet* bullet = new PlayerBullet;
	bullet->set_texture(texture);
	if (bullet->init())
	{
		
		bullet->set_position(position);
		bullet->set_rotation(1.57f);
		if (on_beat) {
			bullet->set_scale({ 0.9f,0.9f });
		}
		bullet->set_on_beat(on_beat);
		bullet->m_movement_dir = normalize({ 0.f, -1.f });

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
	if (bullet->init())
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
    printf("spawning structure\n");
    Structure* new_structure;
    vec2 scale = {1.f, 1.f};
    switch (type) {
        case HEALING_STRUCTURE:
            new_structure = new Healing_Structure;
            break;
        case BLACK_HOLE_STRUCTURE:
            new_structure = new Black_Hole_Structure;
            scale = 0.5 * scale;
            break;
        case SHOOTING_STRUCTURE:
            new_structure = new Shooting_Structure;
            scale = 3 * scale;
            break;
    }
    new_structure->set_texture(texture);
    if (!(new_structure->init(position, scale, 0))) {
        printf("Issues\n");
        return nullptr;
    }
    switch (type) {
        case HEALING_STRUCTURE:
            ((Healing_Structure*) new_structure)->set_boss(boss);
            break;
    }
    printf("Finished\n");
    return new_structure;
}

void spawn_structure(int type, Boss* boss, vec2 position, Texture* texture, std::vector<Structure*>* structures) {
    Structure* structure = spawn_structure(type, boss, position, texture);
    if (structure != nullptr)
        structures->emplace_back(structure);
}

void spawn_beat_circle(int dir, float pos,
float speed, Player* player,
Texture* texture, std::vector<BeatCircle>* bcs) {
    BeatCircle* bc = spawn_beat_circle(dir, pos, speed, player, texture);
    if (bc != nullptr)
        bcs->emplace_back(*bc);
}

BeatCircle* spawn_beat_circle(int dir, float pos, float speed, Player* player, Texture* texture) {
    BeatCircle* beat_circle = new BeatCircle(player, speed);
    bool type = ((dir % 2) == 1);
    beat_circle->set_texture(texture);
    if (beat_circle->init()) {
        beat_circle->set_dir(dir);
        float angle = 0;
        vec2 spawn_pos = -1*pos * beat_circle->get_movement_dir();
        beat_circle->set_position(spawn_pos);
        beat_circle->set_scale({1.5,1.5});
        return beat_circle;
    }
    fprintf(stderr, "Failed to spawn beat circle");
    return nullptr;
}