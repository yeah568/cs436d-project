//
// Created by Sarah Jade on 3/12/18.
//
#ifdef _MSC_VER
#define data_path "../data"
#else
#define data_path "data"
#endif
#define audio_path(name) data_path "/audio/" name

#include "AudioEngine.hpp"


bool AudioEngine::init() {

    result = FMOD::System_Create(&system);      // Create the main system object.

    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creation of FMOD system failure\n", result);
        return false;
    }

    result = system->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.

    if (result != FMOD_OK) {
        printf("FMOD error! (%d) initialization of FMOD system failure\n", result);
        return false;
    }

    return true;
}

AudioEngine::AudioEngine() {

}

AudioEngine::~AudioEngine() {

}

bool AudioEngine::destroy() {

    result = music_level->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing music\n", result);
        return false;
    }

    result = sound_player_hit->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_player_hit\n", result);
        return false;
    }

    result = sound_boss_hit->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_boss_hit\n", result);
        return false;
    }
    result = sound_enemy_hit->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_enemy_hit\n", result);
        return false;
    }
    result = sound_boss_death->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_boss_death\n", result);
        return false;
    }
    result = sound_player_death->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_player_death\n", result);
        return false;
    }
    result = sound_structure_death->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_structure_death\n", result);
        return false;
    }
    result = sound_perfect_timing->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_perfect_timing\n", result);
        return false;
    }
    result = sound_good_timing->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_good_timing\n", result);
        return false;
    }
    result = sound_bad_timing->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing sound_bad_timing\n", result);
        return false;
    }
    result = system->close();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error closing system\n", result);
        return false;
    }
    result = system->release();
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) error releasing system\n", result);
        return false;
    }
    return true;

}

bool AudioEngine::load_sounds() {

    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_player_hit);

    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_player_hit FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_boss_hit);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_boss_hit FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_boss_death);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_boss_death FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_player_death);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_player_death FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_enemy_hit);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_enemy_hit FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_structure_death);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_structure_death FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_perfect_timing);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_perfect_timing FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_good_timing);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_good_timing FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_bad_timing);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound_bad_timing FMOD failure\n", result);
        return false;
    }

    return true;

}

bool AudioEngine::load_music(std::string song_path) {

    //can turn on looping for songs?

    result = system->createSound(song_path.c_str(), FMOD_DEFAULT, 0,
                                 &music_level);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    return true;
}

bool AudioEngine::update() {
    return system->update();
}

bool AudioEngine::is_playing(FMOD::Channel *channel) {
    return FMOD_OK == channel->isPlaying(is_Playing);
}

void AudioEngine::play_music() {
    system->playSound(music_level, 0, false, &music_channel);
}

FMOD::Channel *AudioEngine::get_music_channel() {
    return music_channel;
}

void AudioEngine::play_boss_hit() {
    system->playSound(sound_boss_hit, 0, false, &sound_effects);
}

void AudioEngine::play_boss_death() {
    system->playSound(sound_boss_death, 0, false, &sound_effects);
}

void AudioEngine::play_player_hit() {
    system->playSound(sound_player_hit, 0, false, &sound_effects);
}

void AudioEngine::play_player_death() {
    system->playSound(sound_player_death, 0, false, &sound_effects);
}

void AudioEngine::play_enemy_hit() {
    system->playSound(sound_enemy_hit, 0, false, &sound_effects);
}

void AudioEngine::play_structure_death() {
    system->playSound(sound_structure_death, 0, false, &sound_effects);
}

void AudioEngine::play_perfect_timing() {
    system->playSound(sound_perfect_timing, 0, false, &sound_effects);
}

void AudioEngine::play_good_timing() {
    system->playSound(sound_good_timing, 0, false, &sound_effects);
}

void AudioEngine::play_bad_timing() {
    system->playSound(sound_bad_timing, 0, false, &sound_effects);
}
