#pragma once

// internal
#include "common.hpp"
#include "Player.hpp"
#include "Turtle.hpp"
#include "Bullet.hpp"
#include "Background.hpp"
#include "BeatList.hpp"
#include "BeatCircle.hpp"
#include "CenterBeatCircle.hpp"
#include "Boss.hpp"
#include "HealthBar.hpp"
#include "Enemy.hpp"
#include "ParticleEmitter.hpp"

// stlib
#include <vector>
#include <random>
#include <unordered_map>

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
class Level {

public:
	Level(int width, int height);
	~Level();
	// Creates a window, sets up events and begins the game
	virtual bool init() = 0;

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();
	int getBossHealth();
	void drawBackground();

	// Should the game be over ?
	bool is_over()const;

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(int key, int action, int mod);
	void on_mouse_move(double xpos, double ypos);

	void load_textures();

	int new_points;

	std::unordered_map<std::string, Texture*> m_textures;

private:
	// Generates a new turtle
	bool spawn_turtle();

	
	// Generates a new fish
	bool spawn_bullet(vec2 position, float angle, bool bullet_type, bool on_beat);

	bool spawn_beat_circle(int dir, float pos, float speed);
	
	void handle_beat(float remaining_offset, Beat* curBeat, vec2 screen);

	//bool spawn_enemy(vec2 position);

	bool spawn_little_enemy();

protected:
	static Texture background_texture;
	
	static CenterBeatCircle blue_center_beat_circle;
	static CenterBeatCircle orange_center_beat_circle;
	static Player m_player;
	unsigned int m_points;

	// C++ rng
	static std::default_random_engine m_rng;
	static std::uniform_real_distribution<float> m_dist; // default 0..1

	HealthBar healthbar;
	int finished = 0;
	BeatList* beatlist;
	int beatPos = 0;
	int m_song;
	GLFWwindow* window;
	// Game entities
	std::vector<Bullet> m_bullets;
	Background m_background;
	vec2 screen;
	std::vector<BeatCircle> m_beatcircles;

	float m_current_speed;
	float m_next_little_enemies_spawn;

	Mix_Music* m_background_music;
	Mix_Chunk* m_player_dead_sound;
	Mix_Chunk* m_player_eat_sound;
	
	std::vector <LittleEnemy> m_little_enemies;
	
	Boss m_boss;

	std::vector<ParticleEmitter*> m_particle_emitters;
};

class Level1 : public Level {
	public:
		Level1(int width, int height) : Level(width, height) {};
		~Level1();
		bool init();
};

class Level2 : public Level {
	public:
		Level2(int width, int height) : Level(width, height) {};
		~Level2();
		bool init();
};
