//
// Created by Sarah Jade on 2/1/18.
//
#pragma once
#ifndef CS436D_PROJECT_SOUNDS_PLAYER_H
#define CS436D_PROJECT_SOUNDS_PLAYER_H


#define SDL_MAIN_HANDLED

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "common.hpp"

class Sounds_Player {
public:
    Sounds_Player();

    ~Sounds_Player();

    bool load_sounds();

    void free_sounds();

    void close_audio();

    void play_level_one_music();

    void play_enemy_death_sound();

    void play_player_death_sound();

private:

    Mix_Music *background_music_level_one;
//    Mix_Music *background_music_level_two;
//    Mix_Music *background_music_level_three;

//    Mix_Music *background_music_main_menu;

    Mix_Chunk *player_death_sound;
//    Mix_Chunk *player_shot_sound;
//    Mix_Chunk *player_shoot_sound;
//    Mix_Chunk *player_aligned_beat_sound;

//    Mix_Chunk *enemy_shoot_sound;
//    Mix_Chunk *enemy_shot_sound;
    Mix_Chunk *enemy_death_sound;
//    Mix_Chunk *enemy_power_up_sound;
};


#endif //CS436D_PROJECT_SOUNDS_PLAYER_H
