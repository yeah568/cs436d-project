//
// Created by Sarah Jade on 3/12/18.
//

#ifndef CS436D_PROJECT_AUDIOENGINE_HPP
#define CS436D_PROJECT_AUDIOENGINE_HPP

#include "../ext/fmod/inc/fmod.hpp"
#include "../ext/fmod/inc/fmod_errors.h"
#include "../ext/fmod/inc/fmod_dsp.h"
#include "../ext/fmod/inc/fmod_dsp_effects.h"

#include <string>


class AudioEngine {

public:

    AudioEngine();

    ~AudioEngine();

    bool init();

    bool destroy();

    bool load_sounds();

    bool load_music(std::string song_path);

    bool load_dsp();

    bool update();

    void play_music();

    FMOD::Channel *get_music_channel();

    bool is_playing(FMOD::Channel *channel);

    void set_distortion_bypass(bool toggle);

    void set_wetness_levels(float wetness);

    void play_boss_hit();

    void play_boss_death();

    void play_player_hit();

    void play_player_death();

    void play_enemy_hit();

    void play_structure_death();

    void play_perfect_timing();

    void play_good_timing();

    void play_bad_timing();

    void set_music_pause(bool toggle);

    void get_music_length(unsigned int *length, FMOD_TIMEUNIT lengthtype);

private:

    FMOD_RESULT result = FMOD_OK;

    bool *is_Playing = new bool(false);

    FMOD::Channel *music_channel = nullptr;
    FMOD::Channel *sound_effects = nullptr;
    FMOD::Sound *sound_player_hit;
    FMOD::Sound *music_level;
    FMOD::Sound *sound_boss_hit;
    FMOD::Sound *sound_enemy_hit;
    FMOD::Sound *sound_boss_death;
    FMOD::Sound *sound_player_death;
    FMOD::Sound *sound_structure_death;
    FMOD::Sound *sound_perfect_timing;
    FMOD::Sound *sound_good_timing;
    FMOD::Sound *sound_bad_timing;
    FMOD::System *system;

//    FMOD::DSP *dsphighpass = 0;
    FMOD::DSP *dspdistortion = 0;
    FMOD::DSP *dspreverb = 0;
//    FMOD::DSP *dsplowpass = 0;

    FMOD::ChannelGroup *music_channel_group;
    FMOD::ChannelGroup *effects_channel_group;
    FMOD::ChannelGroup *master_channel_group;
};


#endif //CS436D_PROJECT_AUDIOENGINE_HPP
