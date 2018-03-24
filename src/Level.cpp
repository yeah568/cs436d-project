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

bool Level::init(std::string song_path1, std::string osu_path, float boss_health_multiplier, float player_health) {
    OsuParser *parser;
    parser = new OsuParser(osu_path.c_str());

    OsuBeatmap beatmap = parser->parse();
    beatlist = new BeatList(beatmap);

	m_big_noodle_renderer = new TextRenderer("BigNoodleTooOblique.ttf", 48);

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
	unsigned int boss_health = 0;
	music_level->getLength(&boss_health, FMOD_TIMEUNIT_MS);
	boss_health *= boss_health_multiplier/1000;
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
	m_combo = 0;

	m_level_state = RUNNING;
	finished = false;

	bpm = 0;
    healthbar.set_texture(tm->get_texture("healthbar"));
    healthbar.init();
	bbox hp_bbox = healthbar.get_bounding_box();
    healthbar.set_scale({1.0f, 1.0f});
    healthbar.set_position({ (hp_bbox.max_x - hp_bbox.min_x) / 2.0f, 50 });
    healthbar.set_rotation(0);
    m_background.init();
	m_background.set_position({ (float)screen.x/ 2, (float)screen.y / 2 });
	m_background.set_scale({ 1.f, 1.f });
	m_background.set_rotation(0);

	bbox temp3 = m_background.get_bounding_box();
	float bg_width = temp3.max_x - temp3.min_x;
	float bg_height = temp3.max_y - temp3.min_y;
	m_background.set_scale({screen.x/bg_width,screen.y/bg_height});

    if (!m_player.init()) {
        return false;
    }
	max_player_health = player_health;
    m_player.set_health_abs(max_player_health);
	healthbar.update(1.f);
    if (m_boss.init((float)boss_health, &m_little_enemies, &m_structures)) {
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
	delete m_big_noodle_renderer;
}
bool Level3::init() {
    m_background.set_texture(tm->get_texture("pokemon_background"));
    return Level::init(song_path("PokemonTheme/00_poketv1open.mp3"),
                       song_path("PokemonTheme/Jason Paige - Pokemon Theme (TV Edit) (Ekaru) [Normal].osu"),
                       2.f, 10.f);
}
bool Level2::init() {
	m_background.set_texture(tm->get_texture("marblesoda_background"));
    return Level::init(song_path("598830 Shawn Wasabi - Marble Soda/Marble Soda.wav"),
                       song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Normal].osu"),
                       1.5f, 10.f);
}

// World initialization
bool Level1::init() {
	m_background.set_texture(tm->get_texture("blends_background"));
    return Level::init(song_path("BlendS/BlendS.wav"),
                       song_path("BlendS/Blend A - Bon Appetit S (Meg) [Easy].osu"),
                       1.f, 10.f);
}

// Releases all the associated resources
void Level::destroy() {
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

    //m_player.set_scale(1.5f);
    m_boss.on_beat(curBeat, screen);
}

// Update our game world

bool Level::update(float elapsed_ms)
{
	if (m_level_state != RUNNING) {
		return true;
	}

 	handle_controller(elapsed_ms);

	if ( music_channel == nullptr || FMOD_OK != music_channel->isPlaying(isPlaying)) {
	  system->playSound(music_level, 0, false, &music_channel);
	  music_channel->setVolume(0.5);
	}
	else {
		m_current_time += elapsed_ms;
	}
	bpm += elapsed_ms;
	printf("Powerup 2 %f\n", m_player.powerUp2_time);
	if (bpm > 400.f) {
		if (m_player.powerUp1_time > 0 || m_player.powerUp2_time > 0 || m_player.powerUp3_time > 0) {
			m_player.set_scale(1.5f);
			bpm = 0;
		}
		if (m_player.powerUp1_time > 0) {
			if (m_player.m_health < 10) {
				m_player.m_health += 1;
				healthbar.update(m_player.get_health() / max_player_health);
			}
		}
		if (m_player.powerUp3_time > 1) {
			printf("its greater than 0");
			Texture *texture = tm->get_texture(m_player.bullet_type ? "bullet_1" : "bullet_2");
			spawn_player_bullet(m_player.get_position(), m_player.get_rotation(), { 0.5, 0.5 }, 10.f, 500.f, texture, &m_bullets);
			m_player.bullet_type = !m_player.bullet_type;
			spawn_player_bullet(m_player.get_position(), m_player.get_rotation(), { 0.5, 0.5 }, 10.f, 500.f, texture, &m_bullets);
			m_player.bullet_type = !m_player.bullet_type;
			bpm = 0;
			
		};
			
			
	}
	
	m_boss_health_bar.set_health_percentage(m_boss.get_health()/m_boss.get_total_health());
	float remaining_offset = elapsed_ms;

	Beat* curBeat;
	while (beatPos < beatlist->beats.size()) {
		curBeat = &beatlist->beats.at(beatPos);

		// We should spawn a beat circle such that when the beat circle gets to the
		// center circle, this event coincides with
		// curBeat->offset <= remaining_offset
		float some_fixed_spawn_distance = 650.0f;
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
			//printf("MISS\n");
			m_combo = 0;
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
				new_points += 100;
				m_level_state = WON;
				music_channel->setPaused(true);
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
  		vibrate_controller(0, 500.f, 32000, 32000);
      system->playSound(sound_player_hit, 0, false, &channel);
			healthbar.update(m_player.get_health() / max_player_health);
			auto pe = new ParticleEmitter(
				little_enemy_it->get_position(),
				100,
				false);
			pe->init();
			little_enemy_it = m_little_enemies.erase(little_enemy_it);
			m_particle_emitters.emplace_back(pe);
			m_player.set_health(-0.5);
			if (m_player.get_health() <= 0) {
				system->playSound(sound_player_death, 0, false, &channel);
				m_player.kill();
				m_level_state = LOST;
				music_channel->setPaused(true);
				return true;
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
							m_boss.remove_structure(*structure_it, m_current_time);
							switch ((*structure_it)->type) {
								case 0:
									m_player.powerUp1_time = 5000;
								case 1:
									m_player.powerUp3_time = 5000;
								case 2:
									m_player.powerUp2_time = 5000;
							}
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

void Level::on_mouse_scroll(GLFWwindow* window, vec2 offset) {

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

	m_big_noodle_renderer->setPosition({ 0 + 10, h - 10 });
	m_big_noodle_renderer->setColour({ 0.85f, 0.85f, 0.85f });
	m_big_noodle_renderer->renderString(projection_2D, std::to_string(m_combo) + "x");




	if (m_level_state != RUNNING) {
		float width;
		std::string text;
		if (m_level_state == LOST) {
			text = "YOU LOSE";
		}
		else if (m_level_state == WON) {
			text = "YOU WIN";
		}

		width = m_big_noodle_renderer->get_width_of_string(text) * 0.5;
		m_big_noodle_renderer->setPosition({ screen.x / 2.f - width, screen.y / 2.f });
		m_big_noodle_renderer->renderString(projection_2D, text);

		width = m_big_noodle_renderer->get_width_of_string("Press SPACE to continue") * 0.5;
		m_big_noodle_renderer->setPosition({ screen.x / 2.f - width, screen.y / 2.f + 50 });
		m_big_noodle_renderer->renderString(projection_2D, "Press SPACE to continue");
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
			case GLFW_KEY_SPACE:
				if (m_level_state == LOST || m_level_state == WON) {
					finished = true;
				}
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
            //printf("PERFECT with delta %f\n", delta);
            spawn_player_bullet(salmon_pos, player_angle, {0.5, 0.5}, 10.f, 5000.f, texture, &m_bullets);
            system->playSound(sound_perfect_timing, 0, false, &channel);
            m_beatcircles.erase(beatcircle_it);
            m_player.bullet_type = !m_player.bullet_type;
			m_combo++;
            break;
        } else if (delta <= good_timing) {
            //printf("GOOD with delta %f\n", delta);
            spawn_player_bullet(salmon_pos, player_angle, {0.5, 0.5}, 5.f, 1500.f, texture, &m_bullets);
            system->playSound(sound_good_timing, 0, false, &channel);
            m_beatcircles.erase(beatcircle_it);
            m_player.bullet_type = !m_player.bullet_type;
			m_combo++;
            break;
        } else if (delta <= bad_timing) {
            //printf("BAD with delta %f\n", delta);
            spawn_player_bullet(salmon_pos, player_angle, {1.f, 1.f}, 3.f, 800.f, texture, &m_bullets);
            system->playSound(sound_bad_timing, 0, false, &channel);
            m_beatcircles.erase(beatcircle_it);
            m_player.bullet_type = !m_player.bullet_type;
			m_combo = 0;
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


void Level::handle_controller(float elapsed_ms) {
#if _WIN32
	ZeroMemory(&controller_state, sizeof(XINPUT_STATE));

	if (XInputGetState(0, &controller_state) != ERROR_SUCCESS) {
		return;
	}

	bool A_button_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
	bool B_button_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
	bool X_button_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
	bool Y_button_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
	bool dpad_down_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
	bool dpad_right_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
	bool dpad_up_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
	bool dpad_left_pressed = ((controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);

	if (A_button_pressed || dpad_down_pressed) {
		on_arrow_key(down);
	}
	if (B_button_pressed || dpad_right_pressed) {
		on_arrow_key(right);
	}
	if (Y_button_pressed || dpad_up_pressed) {
		on_arrow_key(up);
	}
	if (X_button_pressed || dpad_left_pressed) {
		on_arrow_key(left);
	}

	float leftTrigger = controller_state.Gamepad.bLeftTrigger / 255.f;
	float rightTrigger = controller_state.Gamepad.bRightTrigger / 255.f;

	float LX = controller_state.Gamepad.sThumbLX;

	//determine how far the controller is pushed
	float magnitude = std::abs(LX);

	//determine the direction the controller is pushed
	float normalizedLX = LX / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}

	m_player.set_movement_dir({ normalizedLX * normalizedMagnitude, 0.f });

	vibration_remaining -= elapsed_ms;
	if (vibration_remaining <= 0) {
		vibrate_controller(0, 0, 0, 0);
	}
#endif
	return;
}

void Level::vibrate_controller(int controller, float duration, unsigned short left_speed, unsigned short right_speed) {
#if _WIN32
	vibration_remaining = duration;
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = left_speed; // use any value between 0-65535 here
	vibration.wRightMotorSpeed = right_speed; // use any value between 0-65535 here
	XInputSetState(controller, &vibration);
#endif;
	return;
}


float Level::getBossHealth() {
    return m_boss.get_health();
}
