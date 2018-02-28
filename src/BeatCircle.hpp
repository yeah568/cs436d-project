#pragma once
#include "common.hpp"
#include "player.hpp"

// Salmon food
class BeatCircle : public Sprite
{
public:
  BeatCircle(Player* player, float speed);
	
	void update(float ms);

	void set_dir(int direction);

  const vec2& get_local_position() const;
  int get_dir() const;
  const vec2& get_movement_dir() const;

private:
  Player* m_player;
	float m_speed;
	vec2 m_local_position; // local coordinates
	int m_dir;
	vec2 m_movement_dir;
};
