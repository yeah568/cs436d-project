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
#include "OsuParser.hpp"

#include "LittleEnemy.hpp"
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
class World {

    static Texture background_texture;
	static OsuBeatmap beatmap;

public:
    World();

    ~World();

    // Creates a window, sets up events and begins the game
    bool init(vec2 screen);

    // Releases all associated resources
    void destroy();

    // Steps the game ahead by ms milliseconds
    bool update(float ms);

    // Renders our scene
    void draw();

    void drawBackground();

    // Should the game be over ?
    bool is_over() const;

private:
    // Generates a new turtle
    bool spawn_turtle();

    bool spawn_little_enemy();

    // Generates a new fish
    bool spawn_bullet(vec2 position, float angle, bool bullet_type, bool on_beat);


    bool spawn_beat_circle(int dir, float pos, float speed);

    // !!! INPUT CALLBACK FUNCTIONS
    void on_key(GLFWwindow *, int key, int, int action, int mod);

    void on_mouse_move(GLFWwindow *window, double xpos, double ypos);

private:
    // Window hjandle
    GLFWwindow *m_window;

    // Number of fish eaten by the salmon, displayed in the window title
    unsigned int m_points;

    BeatList *beatlist;
    int beatPos = 0;

	// Game entities
	Player m_salmon;
	std::vector<Turtle> m_turtles;
	std::vector <LittleEnemy> m_little_enemies;
	std::vector<Bullet> m_bullets;
	Background m_background;
	Boss m_boss;

	
	std::vector<BeatCircle> m_beatcircles;

    void handle_beat(float remaining_offset, Beat *curBeat, vec2 screen);

    float m_current_speed;
    float m_next_turtle_spawn;
    float m_next_fish_spawn;
    float m_next_little_enemies_spawn;

    CenterBeatCircle blue_center_beat_circle;
    CenterBeatCircle orange_center_beat_circle;

    Mix_Music *m_background_music;
    Mix_Chunk *m_salmon_dead_sound;
    Mix_Chunk *m_salmon_eat_sound;

    // C++ rng
    std::default_random_engine m_rng;
    std::uniform_real_distribution<float> m_dist; // default 0..1
};
