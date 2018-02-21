#pragma once

// internal
#include "common.hpp"
#include "player.hpp"
#include "turtle.hpp"
#include "bullet.hpp"
#include "background.hpp"
#include "BeatList.hpp"
#include "BeatCircle.hpp"
#include "CenterBeatCircle.hpp"
#include "boss.hpp"
#include "enemy.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#if defined(__linux__) || defined(LINUX)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#endif

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class Level
{
	static Texture background_texture;
	static vec2 screen;
	static CenterBeatCircle blue_center_beat_circle;
	static CenterBeatCircle orange_center_beat_circle;
	static Player m_player;
	static unsigned int m_points;
	// C++ rng
	static std::default_random_engine m_rng;
	static std::uniform_real_distribution<float> m_dist; // default 0..1

public:
	Level();
	~Level();

	// Creates a window, sets up events and begins the game
	bool init();

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	void drawBackground();

	// Should the game be over ?
	bool is_over()const;

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(int key, int action, int mod);
	void on_mouse_move(double xpos, double ypos);

private:
	// Generates a new turtle
	bool spawn_turtle();

	// Generates a new fish
	bool spawn_bullet(vec2 position, float angle, bool bullet_type, bool on_beat);

	bool spawn_beat_circle(int dir, float pos, float speed);
	
	void handle_beat(float remaining_offset, Beat* curBeat, vec2 screen);

private:

	BeatList* beatlist;
	int beatPos = 0;

	// Game entities
	std::vector<Turtle> m_turtles;
	std::vector<Bullet> m_bullets;
	Background m_background;

	std::vector<BeatCircle> m_beatcircles;

	float m_current_speed;

	Mix_Music* m_background_music;
	Mix_Chunk* m_player_dead_sound;
	Mix_Chunk* m_player_eat_sound;

	std::vector<Enemy> enemies;
	Boss boss;
};
