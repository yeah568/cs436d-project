//
// Created by Sarah Jade on 2/1/18.
//

#include "sounds_player.h"

Sounds_Player::Sounds_Player() {

}

Sounds_Player::~Sounds_Player() {

}

bool Sounds_Player::load_sounds() {

    // Loading music and sounds
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL Audio");
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        fprintf(stderr, "Failed to open audio device");
        return false;
    }

    background_music_level_one = Mix_LoadMUS(audio_path("music.wav"));
    player_death_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
    enemy_death_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));

    if (background_music_level_one == nullptr || player_death_sound == nullptr || enemy_death_sound == nullptr) {
        fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
        return false;
    }

    fprintf(stderr, "Loaded music");
    return true;

}

void Sounds_Player::play_level_one_music() {

    Mix_PlayMusic(background_music_level_one, -1);

}

void Sounds_Player::free_sounds() {

    if (background_music_level_one != nullptr)
        Mix_FreeMusic(background_music_level_one);
    if (player_death_sound != nullptr)
        Mix_FreeChunk(player_death_sound);
    if (enemy_death_sound != nullptr)
        Mix_FreeChunk(enemy_death_sound);
}

void Sounds_Player::close_audio() {
    Mix_CloseAudio();
}

void Sounds_Player::play_enemy_death_sound() {
    Mix_PlayChannel(-1, enemy_death_sound, 0);
}


void Sounds_Player::play_player_death_sound() {
    Mix_PlayChannel(-1, player_death_sound, 0);
}