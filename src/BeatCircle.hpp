#pragma once
#include "common.hpp"
#include "Player.hpp"

// Salmon food
class BeatCircle : public Sprite
{
public:
  BeatCircle(Player* player, float speed, float abs_offset);
	
	void update(float ms);

	void set_dir(int direction, float pos);

  const vec2& get_local_position() const;
  int get_dir() const;
  const vec2& get_movement_dir() const;
  float get_offset() const;

private:
  Player* m_player;
	float m_speed;
	vec2 m_local_position; // local coordinates
	int m_dir;
	vec2 m_movement_dir;
	float m_abs_offset;
};
