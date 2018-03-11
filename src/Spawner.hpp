#pragma once

#ifndef Spawner_H
#define Spawner_H

#include "Bullet.hpp"
#include "Enemy.hpp"
#include "BeatCircle.hpp"
#include "Structure.hpp"

void spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, Texture* texture, std::vector<PlayerBullet>* bullets);
PlayerBullet* spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, Texture* texture);

void spawn_enemy_bullet(vec2 position, vec2 movement_dir, Texture* texture, std::vector<EnemyBullet>* bullets);
EnemyBullet* spawn_enemy_bullet(vec2 position, vec2 movement_dir, Texture* texture);

LittleEnemy* spawn_little_enemy(vec2 position, Texture* texture);
void spawn_little_enemy(vec2 position, Texture* texture, std::vector<LittleEnemy>* enemies);

Structure* spawn_structure(int type, Boss* boss, vec2 position, Texture* texture);
void spawn_structure(int type, Boss* boss, vec2 position, Texture* texture, std::vector<Structure*>* structures);

void spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, Player* player, Texture* texture, std::vector<BeatCircle>* bcs);
BeatCircle* spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, Player* player, Texture* texture);
#endif