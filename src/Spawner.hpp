#pragma once

#ifndef Spawner_H
#define Spawner_H

#include "Bullet.hpp"
#include "Enemy.hpp"
#include "BeatCircle.hpp"
#include "Structure.hpp"

void spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<PlayerBullet>> bullets);
std::shared_ptr<PlayerBullet> spawn_player_bullet(vec2 position, float angle, vec2 scale, float dmg, float spd, std::shared_ptr<Texture> texture);

void spawn_enemy_bullet(vec2 position, vec2 movement_dir, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<EnemyBullet>> bullets);
std::shared_ptr<EnemyBullet> spawn_enemy_bullet(vec2 position, vec2 movement_dir, std::shared_ptr<Texture> texture);

std::shared_ptr<LittleEnemy> spawn_little_enemy(vec2 position, std::shared_ptr<Texture> texture);
void spawn_little_enemy(vec2 position, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<LittleEnemy>> enemies);

std::shared_ptr<Structure> spawn_structure(int type, std::shared_ptr<Texture> texture);
void spawn_structure(int type, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<std::shared_ptr<Structure>>> structures);

void spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, std::shared_ptr<Texture> texture, std::shared_ptr<std::vector<BeatCircle>> bcs);
std::shared_ptr<BeatCircle> spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset, std::shared_ptr<Texture> texture);
#endif