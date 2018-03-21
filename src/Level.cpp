#include "common.hpp"
#include "Level.hpp"
#include "OsuParser.hpp"
#include "BeatCircle.hpp"
#include "Bullet.hpp"
#include "SpriteSheet.hpp"
#include "Structure.hpp"
#include "Spawner.hpp"
#include "TextureManager.hpp"


// stlib
#include <string.h>
#include <cassert>
#include <sstream>

Texture Level::background_texture;

CenterBeatCircle Level::blue_center_beat_circle;
CenterBeatCircle Level::orange_center_beat_circle;
Player Level::m_player;

//C++_rng
std::default_random_engine Level::m_rng;
std::uniform_real_distribution<float> Level::m_dist;//default:0..1

bool Level::show_hitboxes = false;

// Same as static in c, local to compilation unit
namespace {
    namespace {
        void glfw_err_cb(int error, const char *desc) {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

Level::Level(float width, float height) : m_points(0), m_next_little_enemies_spawn(0.f) {
    screen.x = width;
    screen.y = height;
    m_rng = std::default_random_engine(std::random_device()());
    tm = TextureManager::get_instance();
    m_player.set_texture(tm->get_texture("character"));
    m_boss.set_texture(tm->get_texture("boss0"));
    m_boss_health_bar.set_texture(tm->get_texture("boss_health_bar"));
    m_points = 0;
    m_current_time = 0;
}

bool Level::init(std::string song_path1, std::string osu_path, float boss_health) {
    OsuParser *parser;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Loading music and sounds
    /*
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL Audio");
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        fprintf(stderr, "Failed to open audio device");
        return false;
    }
    */

//    m_background_music = Mix_LoadMUS(song_path.c_str());
    parser = new OsuParser(osu_path.c_str());

    OsuBeatmap beatmap = parser->parse();
    beatlist = new BeatList(beatmap);
    /*
    if (!m_background_music) {
        printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
        // this might be a critical error...
    }

    m_player_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
    m_player_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));

    if (m_background_music == nullptr || m_player_dead_sound == nullptr || m_player_eat_sound == nullptr) {
        fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
        return false;
    }

    fprintf(stderr, "Loaded music through SDL");
*/


    FMOD_RESULT result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creation of FMOD system failure\n", result);
        return false;
    }
    result = system->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) initialization of FMOD system failure\n", result);
        return false;
    }
    music_channel = nullptr;
    channel = nullptr;
    isPlaying = new bool(false);
    //can turn on looping for songs?
    result = system->createSound(song_path1.c_str(), FMOD_DEFAULT, 0,
                                 &music_level);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_player_hit);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_boss_hit);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_boss_death);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_player_death);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_enemy_hit);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_structure_death);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_perfect_timing);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_good_timing);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }
    result = system->createSound(audio_path("345551_enemy_Spawn.wav"), FMOD_DEFAULT, 0, &sound_bad_timing);
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) creating sound FMOD failure\n", result);
        return false;
    }

    printf("FMOD loaded sounds and music");


    m_current_speed = 1.f;

    m_background.init();


    healthbar.set_texture(tm->get_texture("healthbar"));
    healthbar.init(5);

    healthbar.set_scale({0.6f, 0.7f});
    healthbar.set_position({200, 50});
    healthbar.set_rotation(0);
    m_background.init();


    if (!m_player.init()) {
        return false;
    }
    m_player.set_health(5);
    if (m_boss.init(boss_health, &m_little_enemies, &m_structures)) {
        Structure::player_bullets = &m_bullets;
        Structure::enemy_bullets = &m_enemy_bullets;
        Structure::player = &m_player;
        m_boss_health_bar.init();
        m_boss_health_bar.set_rotation(0);
        bbox bhp_bbox = m_boss_health_bar.get_bounding_box();
        m_boss_health_bar.set_position({screen.x / 2.0f, (bhp_bbox.max_y + bhp_bbox.min_y) / 2.0f});
        blue_center_beat_circle.init(false);
        orange_center_beat_circle.init(true);
        CenterBeatCircle::player = &m_player;
        LittleEnemy::player = &m_player;

        return true;
    }

    return false;
}

Level::~Level() {

}

bool Level2::init() {
    return Level::init(song_path("598830 Shawn Wasabi - Marble Soda/Marble Soda.wav"),
                       song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Normal].osu"),
                       375.0f);
}

// World initialization
bool Level1::init() {
    return Level::init(song_path("BlendS/BlendS.wav"),
                       song_path("BlendS/Blend A - Bon Appetit S (Meg) [Easy].osu"),
                       250.0f);
}

// Releases all the associated resources
void Level::destroy() {
    /*
    if (m_background_music != nullptr)
        Mix_FreeMusic(m_background_music);
    if (m_player_dead_sound != nullptr)
        Mix_FreeChunk(m_player_dead_sound);
    if (m_player_eat_sound != nullptr)
        Mix_FreeChunk(m_player_eat_sound);

    Mix_CloseAudio();
    */

    music_level->release();
    sound_player_hit->release();
    sound_boss_hit->release();
    sound_enemy_hit->release();
    sound_boss_death->release();
    sound_player_death->release();
    sound_structure_death->release();
    sound_perfect_timing->release();
    sound_good_timing->release();
    sound_bad_timing->release();
    system->close();
    system->release();


    m_player.destroy();
    m_boss.destroy();

    for (auto &bullet : m_bullets)
        bullet.destroy();
    for (auto &beatcircle : m_beatcircles)
        beatcircle.destroy();
    for (auto &enemy : m_little_enemies)
        enemy.destroy();
    for (auto &structure : m_structures)
        structure->destroy();
    for (auto &bullet : m_enemy_bullets)
        bullet.destroy();
    m_enemy_bullets.clear();
    orange_center_beat_circle.destroy();
    blue_center_beat_circle.destroy();
    healthbar.destroy();
    m_boss_health_bar.destroy();
    m_bullets.clear();
    m_little_enemies.clear();
    m_structures.clear();
    m_beatcircles.clear();
}


void Level::handle_beat(float remaining_offset, Beat *curBeat, vec2 screen) {
    remaining_offset -= curBeat->relativeOffset;
    //beatPos++;
    // do beat things
    // spawn thing




    // spawn a thing
    //spawn_turtle();
    //Turtle& new_turtle = m_turtles.back();
    //new_turtle.set_position({ ((64.f + (float)curBeat->x) / 640.f)*screen.x, ((48.f + (float)curBeat->y) / 480.f)*screen.y });

    m_player.scale_by(1.3f);
    m_boss.on_beat(curBeat, screen);
}

// Update our game world

bool Level::update(float elapsed_ms)
{
  if ( music_channel == nullptr || FMOD_OK != music_channel->isPlaying(isPlaying)) {
      system->playSound(music_level, 0, false, &music_channel);
	}
	else {
		m_current_time += elapsed_ms;
	}

	m_boss_health_bar.set_health_percentage(m_boss.get_health()/m_boss.get_total_health());
	float remaining_offset = elapsed_ms;

	Beat* curBeat;
	while (beatPos < beatlist->beats.size()) {
		curBeat = &beatlist->beats.at(beatPos);

		// We should spawn a beat circle such that when the beat circle gets to the
		// center circle, this event coincides with
		// curBeat->offset <= remaining_offset
		float some_fixed_spawn_distance = 500.0f;
		float beat_spawn_time = 1668.f;
		float speed = some_fixed_spawn_distance / beat_spawn_time;
		if (curBeat->absoluteOffset <= m_current_time + beat_spawn_time && !curBeat->spawned) {
			float delta = m_current_time - curBeat->absoluteOffset + beat_spawn_time;
			float pos = some_fixed_spawn_distance - speed * delta;
			float scale = 1.5f - delta / 1500.f;
			curBeat->spawned = true;
			Texture* texture = tm->get_texture(((curBeat->dir % 2) == 1) ? "orange_moving_beat" : "blue_moving_beat");
			spawn_beat_circle(curBeat->dir, pos, speed, scale, curBeat->absoluteOffset, &m_player, texture, &m_beatcircles);
		}
		else {
			curBeat->relativeOffset -= remaining_offset;
			//printf("offset: %f\n", curBeat->offset);
			break;
		}
		beatPos++;
	}

	while (lastBeat < beatlist->beats.size()) {
		Beat* b = &beatlist->beats.at(lastBeat);
		if (b->absoluteOffset <= m_current_time) {
			handle_beat(remaining_offset, b, screen);
			lastBeat++;
		}
		else {
			break;
		}
	}

	// Checking player - beatcircle complete overlaps/overshoots
	auto beatcircle_it = m_beatcircles.begin();
	while (beatcircle_it != m_beatcircles.end()) {
		float delta = m_current_time - beatcircle_it->get_offset();
		if (delta > bad_timing) {
			printf("MISS\n");
			beatcircle_it = m_beatcircles.erase(beatcircle_it);
		}
		else {
			++beatcircle_it;
		}
	}

	// Updating all entities, making the turtle and fish
	// faster based on current

	auto bullet_it = m_bullets.begin();
	while (bullet_it != m_bullets.end())
	{
		if (m_boss.collides_with(*bullet_it))
		{
			system->playSound(sound_boss_hit, 0, false, &channel);
			//printf("Boss hit by bullet\n");
			m_boss.set_health(-bullet_it->get_damage());
			m_boss_health_bar.set_health_percentage(m_boss.get_health()/m_boss.get_total_health());
			bullet_it = m_bullets.erase(bullet_it);
			if (m_boss.get_health() <= 0) {
        system->playSound(sound_boss_death, 0, false, &channel);
				finished = 1;
				new_points += 100;
				return true;
			}
			break;
		}
		if (bullet_it->get_bounding_box().max_y < 0) {
			bullet_it = m_bullets.erase(bullet_it);
			break;
		}
		++bullet_it;
	}

	m_player.update(elapsed_ms);
	if (m_player.get_position().y > screen.y)
		exit(0);
	m_boss.update(elapsed_ms, screen, &m_bullets);
	//Enemy::update_player_position(m_player.get_position());
	float elapsed_modified_ms = elapsed_ms * m_current_speed;

	for (auto& bullet : m_bullets)
		bullet.update(elapsed_modified_ms);
	for (auto& beatcircle : m_beatcircles)
		beatcircle.update(elapsed_modified_ms);
	for (auto& enemy : m_little_enemies)
		enemy.update(elapsed_modified_ms);
	for (auto& structure : m_structures) {
		structure->update(elapsed_modified_ms);
	for (auto& bullet : m_enemy_bullets)
		bullet.update(elapsed_modified_ms);
	//printf("Level structures: %d\n", m_structures.size());ctv

		//printf("Updated structure\n");
	}
	for (auto little_enemy_it = m_little_enemies.begin(); little_enemy_it != m_little_enemies.end();) {
		if (m_player.collides_with(*little_enemy_it)) {
      system->playSound(sound_player_hit, 0, false, &channel);
			healthbar.update();
			auto pe = new ParticleEmitter(
				little_enemy_it->get_position(),
				100,
				false);
			pe->init();
			little_enemy_it = m_little_enemies.erase(little_enemy_it);
			m_particle_emitters.emplace_back(pe);
			m_player.set_health(-1);
			if (m_player.get_health() <= 0) {
        system->playSound(sound_player_death, 0, false, &channel);
				//m_player.kill();
				//printf("Player has died\n");
			}
			break;
		} else {
			++little_enemy_it;
		}
	}


	if (m_bullets.size() > 0) {
		for (auto bullet_it = m_bullets.begin(); bullet_it != m_bullets.end();) {
			bool removed_enemy = false;
			bool hit_structure = false;
			if (m_little_enemies.size() > 0){
				for (auto little_enemy_it = m_little_enemies.begin(); little_enemy_it != m_little_enemies.end();) {
					if (little_enemy_it->collides_with(*bullet_it)) {
            system->playSound(sound_enemy_hit, 0, false, &channel);
						auto pe = new ParticleEmitter(
							little_enemy_it->get_position(),
							100,
							false);
						pe->init();
						m_particle_emitters.emplace_back(pe);
						new_points += (bullet_it->get_damage() == 100 ? 15 : 10);
						little_enemy_it = m_little_enemies.erase(little_enemy_it);
						bullet_it = m_bullets.erase(bullet_it);
						removed_enemy = true;

						break;
					}
					else {
						++little_enemy_it;
					}
				}
			}
			if (removed_enemy) {
				continue;
			}
			if (m_structures.size() > 0) {
				for (auto structure_it = m_structures.begin(); structure_it != m_structures.end();) {
					if ((*structure_it)->collides_with(*bullet_it)) {
						bullet_it = m_bullets.erase(bullet_it);
						hit_structure = true;
						(*structure_it)->health--;

						if ((*structure_it)->health < 1) {
							if ((*structure_it) == m_boss.structure_slots.left) {
								m_boss.structure_slots.left = nullptr;
							} else if ((*structure_it) == m_boss.structure_slots.center) {
								m_boss.structure_slots.center = nullptr;
							} else {
								m_boss.structure_slots.right = nullptr;
							}
              system->playSound(sound_structure_death, 0, false, &channel);
							structure_it = m_structures.erase(structure_it);
						}
						break;
					}
					else {
						++structure_it;
					}

				}
			}
			if (!hit_structure) { ++bullet_it; };
		}
	}

	for (auto& particleEmitter : m_particle_emitters) {
		particleEmitter->update(elapsed_ms);
	}

	for (auto pe_it = m_particle_emitters.begin(); pe_it != m_particle_emitters.end();) {
		if ((*pe_it)->get_alive_particles() == 0) {
			delete *pe_it;
			pe_it = m_particle_emitters.erase(pe_it);
		}
		else {
			++pe_it;
		}
	}
  system->update();
	return true;
}

// Render our game world
void Level::draw()
{
	float w = screen.x;
	float h = screen.y;
	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5;
	float top = 0.f;// (float)h * -0.5;
	float right = w;// *0.5;
	float bottom = h;// *0.5;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	m_background.set_position({ w / 2, h / 2 });

	m_background.draw(projection_2D);

	// Drawing entities

	for (auto& bullet : m_bullets)
		bullet.draw(projection_2D);
	for (auto& bullet : m_enemy_bullets)
		bullet.draw(projection_2D);
	for (auto& beatcircle : m_beatcircles)
		beatcircle.draw(projection_2D);

	for (auto& enemy : m_little_enemies)
		enemy.draw(projection_2D);
	for (auto& particleEmitter : m_particle_emitters) {
		particleEmitter->draw(projection_2D);
	}

	for (auto& structure : m_structures) {
		//printf("Drawing: %f\n", structure->get_position().x);
		structure->draw(projection_2D);
	}
	m_boss.draw(projection_2D);
	m_boss_health_bar.draw(projection_2D);
	//healthbar.draw(projection_2D);
	healthbar.draw(projection_2D);
	orange_center_beat_circle.draw(projection_2D);
	blue_center_beat_circle.draw(projection_2D);
	m_player.draw(projection_2D);

	if (show_hitboxes) {
		for (auto& bullet : m_bullets)
			bullet.draw_hitboxes(projection_2D);
		for (auto& beatcircle : m_beatcircles)
			beatcircle.draw_hitboxes(projection_2D);
		for (auto& enemy : m_little_enemies)
			enemy.draw_hitboxes(projection_2D);

		m_boss.draw_hitboxes(projection_2D);
		healthbar.draw_hitboxes(projection_2D);
		m_player.draw_hitboxes(projection_2D);
	}
}

// Should the game be over ?
bool Level::is_over() const {
    // TODO: Implement Me
    return finished;
}

// On key callback
void Level::on_key(int key, int action, int mod) {
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE SALMON MOVEMENT HERE
    // key is of 'type' GLFW_KEY_
    // action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_H:
                finished = 1;
                break;
            case GLFW_KEY_D:
                m_player.add_movement_dir({1.f, 0.f});
                break;
            case GLFW_KEY_A:
                m_player.add_movement_dir({-1.f, 0.f});
                break;
            case GLFW_KEY_LEFT_SHIFT:
                m_player.dash();
                break;
            case GLFW_KEY_U:
                m_player.exploding_timer = 1;
                break;

            case GLFW_KEY_P:
                show_hitboxes = !show_hitboxes;
                break;
            case GLFW_KEY_UP:
            case GLFW_KEY_I:
                on_arrow_key(up);
                break;
            case GLFW_KEY_DOWN:
            case GLFW_KEY_K:
                on_arrow_key(down);
                break;
            case GLFW_KEY_LEFT:
            case GLFW_KEY_J:
                on_arrow_key(left);
                break;
            case GLFW_KEY_RIGHT:
            case GLFW_KEY_L:
                on_arrow_key(right);
                break;
        }
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_D:
                m_player.add_movement_dir({-1.f, 0.f});
                break;
            case GLFW_KEY_A:
                m_player.add_movement_dir({1.f, 0.f});
                break;
                //case GLFW_KEY_UP:
                //case GLFW_KEY_W:
                //	m_player.add_movement_dir({ 0.f, 1.f });
                ///	break;
                //case GLFW_KEY_DOWN:
                //case GLFW_KEY_S:
                //	m_player.add_movement_dir({ 0.f, -1.f });
                //	break;
        }
    }


    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
        m_player.destroy();
        m_player.init();
        m_background.init();

        m_bullets.clear();
        m_current_speed = 1.f;
    }

    // Control the current speed with `<` `>`
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
        m_current_speed -= 0.1f;
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
        m_current_speed += 0.1f;

    m_current_speed = fmax(0.f, m_current_speed);
}

void Level::on_arrow_key(Dir dir) {
    float player_angle = m_player.get_rotation() + 1.57f;
    vec2 salmon_pos = m_player.get_position();
    Texture *texture = tm->get_texture(m_player.bullet_type ? "bullet_1" : "bullet_2");
    auto beatcircle_it = m_beatcircles.begin();
    while (beatcircle_it != m_beatcircles.end()) {
        float abs_offset = beatcircle_it->get_offset();
        float delta = std::abs(m_current_time - abs_offset);
        if (delta > bad_timing) {
            break;
        }

        if (dir != beatcircle_it->get_dir()) {
            beatcircle_it++;
            continue;
        }

        if (delta <= perfect_timing) {
            printf("PERFECT with delta %f\n", delta);
            spawn_player_bullet(salmon_pos, player_angle, {0.5, 0.5}, 10.f, 5000.f, texture, &m_bullets);
            system->playSound(sound_perfect_timing, 0, false, &channel);
            m_beatcircles.erase(beatcircle_it);
            m_player.bullet_type = !m_player.bullet_type;
            break;
        } else if (delta <= good_timing) {
            printf("GOOD with delta %f\n", delta);
            spawn_player_bullet(salmon_pos, player_angle, {0.5, 0.5}, 5.f, 1500.f, texture, &m_bullets);
            system->playSound(sound_good_timing, 0, false, &channel);
            m_beatcircles.erase(beatcircle_it);
            m_player.bullet_type = !m_player.bullet_type;
            break;
        } else if (delta <= bad_timing) {
            printf("BAD with delta %f\n", delta);
            spawn_player_bullet(salmon_pos, player_angle, {1.f, 1.f}, 3.f, 800.f, texture, &m_bullets);
            system->playSound(sound_bad_timing, 0, false, &channel);
            m_beatcircles.erase(beatcircle_it);
            m_player.bullet_type = !m_player.bullet_type;
            break;
        }
        beatcircle_it++;
    }
}

void Level::on_mouse_move(double xpos, double ypos) {
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE SALMON ROTATION HERE
    // xpos and ypos are relative to the top-left of the window, the salmon's
    // default facing direction is (1, 0)
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    m_player.set_mouse((float) xpos, (float) ypos);
}

float Level::getBossHealth() {
    return m_boss.get_health();
}
