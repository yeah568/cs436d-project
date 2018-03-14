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
#include "SpriteSheet.hpp"
#include "Enemy.hpp"
#include "ParticleEmitter.hpp"
#include "BossHealthBar.hpp"
#include "Structure.hpp"
#include "TextureManager.hpp"

// external
//#include "fmod.hpp"
//#include "fmod_errors.h"
#include "fmod.hpp"
#include "fmod_errors.h"

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
    virtual bool init()=0;

    // Releases all associated resources
    void destroy();

    // Steps the game ahead by ms milliseconds
    bool update(float ms);

    // Renders our scene
    void draw();

    int getBossHealth();

    void drawBackground();

    // Should the game be over ?
    bool is_over() const;

    // !!! INPUT CALLBACK FUNCTIONS
    void on_key(int key, int action, int mod);

    void on_mouse_move(double xpos, double ypos);

    int new_points;

private:
    // Generates a new turtle
    bool spawn_turtle();


	//bool spawn_beat_circle(int dir, float pos, float speed);
	
	void handle_beat(float remaining_offset, Beat& curBeat, vec2 screen);

    //bool spawn_enemy(vec2 position);

    //bool spawn_little_enemy();

    void on_arrow_key(Dir dir);

protected:

	mat3 m_projection_2D;
	void calculate_projection_2D();
	bool init(std::string song_path1, std::string osu_path, float boss_health);
	std::shared_ptr<TextureManager> tm;
	Texture background_texture;
	static bool show_hitboxes;
	
	bool initialize_fmod(std::string song_path1);

	void setup_references();
	void set_textures();
	void make_shared_ptrs();

	float m_current_time;
	float m_current_speed;
	float m_next_little_enemies_spawn;

	static CenterBeatCircle blue_center_beat_circle;
	static CenterBeatCircle orange_center_beat_circle;
	static std::shared_ptr<Player> m_player;
	int m_points;

    FMOD::System *system;
    FMOD::Channel *channel;
    FMOD::Channel *music_channel;
    FMOD::Sound *sound_player_hit;
    FMOD::Sound *sound_boss_hit;
    FMOD::Sound *music_level;
    FMOD::Sound *sound_boss_death;
    FMOD::Sound *sound_player_death;
    FMOD::Sound *sound_enemy_hit;
    FMOD::Sound *sound_structure_death;
    FMOD::Sound *sound_perfect_timing;
    FMOD::Sound *sound_good_timing;
    FMOD::Sound *sound_bad_timing;
    std::shared_ptr<bool> isPlaying;

	BossHealthBar m_boss_health_bar;
	HealthBar healthbar;
	int finished = 0;
	SpriteSheet spritesheet;
	std::shared_ptr<BeatList> beatlist;
	int beatPos = 0;
	int lastBeat = 0;
	int m_song;
	GLFWwindow* window;
	// Game entities
	std::shared_ptr<std::vector<PlayerBullet>> m_bullets;
	std::shared_ptr<std::vector<EnemyBullet>> m_enemy_bullets;
	Background m_background;
	vec2 screen;
	std::shared_ptr<std::vector<BeatCircle>> m_beatcircles;

    const int perfect_timing = 40;
    const int good_timing = 80;
    const int bad_timing = 120;

    // C++ rng
    static std::default_random_engine m_rng;
    static std::uniform_real_distribution<float> m_dist; // default 0..1

	
	std::shared_ptr<std::vector <LittleEnemy>> m_little_enemies;
	std::shared_ptr<std::vector<std::shared_ptr<Structure>>> m_structures;
	std::shared_ptr<Boss> m_boss;

	std::vector<std::shared_ptr<ParticleEmitter>> m_particle_emitters;
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
