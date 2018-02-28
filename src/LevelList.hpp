#pragma once

// internal
#include "common.hpp"
#include "Level.hpp"
#include "Background.hpp"

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

class LevelList
{
	std::vector<Level> levels;

public:
	LevelList();
	~LevelList();

	

private:
	



private:
	Level * current_level;
};
