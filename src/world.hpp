#pragma once

// internal
#include "common.hpp"
#include "level.hpp"

#include "enemy.hpp"
// stlib
#include <vector>
#include <string>
#include <unordered_map>
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

public:
    World();

    ~World();

    // Creates a window, sets up events and begins the game
    bool init(vec2 screen);

    // Releases all associated resources
    void destroy();

	void draw();

    // Steps the game ahead by ms milliseconds
    bool update(float ms);

	// Should the game be over ?
	bool is_over()const;

	void incrementLevelCounter() { levelCounter++; };
private:
	
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

    bool spawn_beat_circle(int dir, float pos, float speed);

  void load_textures();

private:
	
	int levelCounter;
	std::vector<Level*> levelList;

	// Window hjandle
	GLFWwindow* m_window;
  	std::unordered_map<std::string, Texture*> m_textures;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
