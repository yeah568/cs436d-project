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

enum LevelStates {
	RUNNING,
	PAUSED,
	LOST,
	WON
};

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class Level {

public:
    Level(float width, float height);

    ~Level();

    // Creates a window, sets up events and begins the game
    virtual bool init()=0;

    // Releases all associated resources
    virtual void destroy();

    // Steps the game ahead by ms milliseconds
    virtual bool update(float ms);

    // Renders our scene
    virtual void draw();

    float getBossHealth();
    static std::vector<Level*>* levelList;

	virtual void on_mouse_scroll(vec2 offset);

    void drawBackground();

    // Should the game be over ?
    bool is_over() const;

    // !!! INPUT CALLBACK FUNCTIONS
    virtual void on_key(int key, int action, int mod);

    virtual void on_mouse_move(double xpos, double ypos);

    virtual void on_mouse_click(vec2 pos) {};

    int new_points;

private:
    // Generates a new fish
    //bool spawn_bullet(vec2 position, float angle, bool bullet_type, bool on_beat);

    //bool spawn_beat_circle(int dir, float pos, float speed);

    void handle_beat(float remaining_offset, Beat *curBeat, vec2 screen);

    //bool spawn_enemy(vec2 position);

    //bool spawn_little_enemy();

    void on_arrow_key(Dir dir);

protected:

    bool init(std::string song_path, std::string osu_path, float boss_health);

    TextureManager *tm;
    static Texture background_texture;
    static bool show_hitboxes;

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
    bool *isPlaying;

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
    int finished = 0;
    SpriteSheet spritesheet;
    BeatList *beatlist;
	std::vector<Beat>::size_type beatPos = 0;
	std::vector<Beat>::size_type lastBeat = 0;
    int m_song;
    GLFWwindow *window;
    // Game entities
    std::vector<PlayerBullet> m_bullets;
    std::vector<EnemyBullet> m_enemy_bullets;
    Background m_background;
    vec2 screen;
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

	TextRenderer* m_comic_sans_renderer;
	TextRenderer* m_exo_renderer;

	LevelStates m_level_state;
	int m_combo;
};

class Level1 : public Level {
public:
    Level1(float width, float height) : Level(width, height) {};

    ~Level1();

    bool init();
};

class Level2 : public Level {
public:
    Level2(float width, float height) : Level(width, height) {};

    ~Level2();

    bool init();
};

class Level3 : public Level {
public:
    Level3(int width, int height) : Level(width, height) {};

    ~Level3();

    bool init();
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
};

class SongSelect : public Level {
public:
    SongSelect(int width, int height) : Level(width, height) {};
    ~SongSelect();
    bool init();
    void destroy();
    bool update(float ms);
    void draw();
	void on_mouse_scroll(vec2 offset);
    // TODO: replace with m_background from Level
    Background background;
    std::vector<Button*> song_boxes;
    Button back_button;
    void on_mouse_click(vec2 pos);
};

#endif
