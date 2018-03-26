#pragma once

#ifndef Level_H
#define Level_H

// internal
#include "common.hpp"
#include "Player.hpp"
#include "Button.hpp"
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
#include "AudioEngine.hpp"
#include "TextRenderer.hpp"

// external
//#include "fmod.hpp"
//#include "fmod_errors.h"
#include "../ext/fmod/inc/fmod.hpp"
#include "../ext/fmod/inc/fmod_errors.h"

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

#if _WIN32
#include <xinput.h>
#endif

enum LevelStates {
	RUNNING,
	PAUSED,
	LOST,
	WON
};

struct LevelTemplate {
    std::string background;
    std::string song_path;
    std::string osu_path;

    // X means X hp per second of song
    float boss_hp_multiplier;
    float player_health;
};
static const LevelTemplate LEVEL3 = {"pokemon_background",song_path("PokemonTheme/00_poketv1open.mp3"),
song_path("PokemonTheme/Jason Paige - Pokemon Theme (TV Edit) (Ekaru) [Normal].osu"),2.f, 10.f};

static const LevelTemplate LEVEL2 = {"marblesoda_background",song_path("598830 Shawn Wasabi - Marble Soda/Marble Soda.wav"),
song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Normal].osu"),1.5f, 10.f};

static const LevelTemplate LEVEL1 = {"blends_background",song_path("BlendS/BlendS.wav"),
song_path("BlendS/Blend A - Bon Appetit S (Meg) [Easy].osu"),1.f, 10.f};

static const std::vector<const LevelTemplate*> ALL_LEVELS({&LEVEL1,&LEVEL2,&LEVEL3});

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class Level {
public:
Level(float width, float height) {screen = {width, height};tm=TextureManager::get_instance();};
virtual bool init() = 0;
virtual void destroy() = 0;
virtual bool update(float ms) = 0;
virtual void draw()=0;
virtual bool is_over() const {return finished==1;};

virtual void on_key(int key, int action, int mod) {};
virtual void on_mouse_move(double xpos, double ypos) {};
virtual void on_mouse_scroll(GLFWwindow* window, vec2 offset) {};
virtual void on_mouse_click(vec2 pos) {};

static std::vector<Level*>* levelList;

protected:
int finished = 0;
vec2 screen;
TextureManager *tm;
Background m_background;
GLFWwindow *window;
};


class GameLevel : public Level {

public:

    GameLevel(float width, float height, const LevelTemplate* lt);

    ~GameLevel();

    // Creates a window, sets up events and begins the game
    bool init();

    // Releases all associated resources
    void destroy();

    // Steps the game ahead by ms milliseconds
    bool update(float ms);

    // Renders our scene
    void draw();

	std::string getScoreString();

    // !!! INPUT CALLBACK FUNCTIONS
    void on_key(int key, int action, int mod);

	void handle_controller(float elapsed_ms);

	void vibrate_controller(int controller, float duration, unsigned short left_speed, unsigned short right_speed);

private:
    void handle_beat(float remaining_offset, Beat *curBeat, vec2 screen);

    void on_arrow_key(Dir dir);

	float vibration_remaining;
#if _WIN32
	XINPUT_STATE controller_state;
#endif

    AudioEngine audioEngine;
    LevelTemplate level_info;
    static bool show_hitboxes;

    static CenterBeatCircle blue_center_beat_circle;
    static CenterBeatCircle orange_center_beat_circle;
    static Player m_player;
    unsigned int m_points;

    const int perfect_timing = 40;
    const int good_timing = 80;
    const int bad_timing = 120;

    // C++ rng
    static std::default_random_engine m_rng;
    static std::uniform_real_distribution<float> m_dist; // default 0..1

    BossHealthBar m_boss_health_bar;
    HealthBar healthbar;
    SpriteSheet spritesheet;
    BeatList *beatlist;

	std::vector<Beat>::size_type beatPos = 0;
	std::vector<Beat>::size_type lastBeat = 0;

    int m_song;
    // Game entities
    std::vector<PlayerBullet> m_bullets;
    std::vector<EnemyBullet> m_enemy_bullets;
    std::vector<BeatCircle> m_beatcircles;

    float m_current_speed;
    float m_next_little_enemies_spawn;

    float m_current_time;

//    Mix_Music *m_background_music;
//    Mix_Chunk *m_player_dead_sound;
//    Mix_Chunk *m_player_eat_sound;

    std::vector<LittleEnemy> m_little_enemies;
    std::vector<Structure *> m_structures;
    Boss m_boss;

	std::vector<ParticleEmitter*> m_particle_emitters;

	TextRenderer* m_big_noodle_renderer;

	LevelStates m_level_state;
	int m_combo;
	float max_player_health;

	unsigned int m_score;
};

class MainMenu : public Level {
public: 
	MainMenu(int width, int height) : Level(width, height) {};
	

	~MainMenu();
	bool init();
	void destroy();
	bool update(float ms);
	void draw();
	Background background;
    Button play_button;
    Button exit_button;
    void on_mouse_click(vec2 pos);
    Button songselect_button;
	void on_mouse_move(double xpos, double ypos);
};

class SongSelect : public Level {
public:
    SongSelect(int width, int height) : Level(width, height) {};
    ~SongSelect();
    bool init();
    void destroy();
    bool update(float ms);
    void draw();
	std::vector<SongButton*> song_boxes;
	Button back_button;
	void on_mouse_click(vec2 pos);
	void on_mouse_scroll(GLFWwindow* window, vec2 offset);
	void on_mouse_move(double xpos, double ypos);
};

#endif
