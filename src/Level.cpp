// Header
#include "common.hpp"
#include "Level.hpp"
#include "OsuParser.hpp"
#include "BeatCircle.hpp"
#include "Bullet.hpp"

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


// Same as static in c, local to compilation unit
namespace
{
	const size_t MAX_TURTLES = 15;
	const size_t MAX_LIL_ENEMIES = 15;
	const size_t MAX_FISH = 5;
	const size_t TURTLE_DELAY_MS = 2000;
	const size_t FISH_DELAY_MS = 5000;
	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

Level::Level(int width, int height)  : m_points(0), m_next_little_enemies_spawn(0.f) {
	screen.x = width;
	screen.y = height;
	m_rng = std::default_random_engine(std::random_device()());
	load_textures();
	m_player.set_texture(m_textures["character"]);
	m_boss.set_texture(m_textures["boss0"]);
	m_points = 0;
	m_current_time = 0;
	}

Level::~Level()
{

}
bool Level2::init() {
	
	OsuParser* parser;
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_background_music = Mix_LoadMUS(song_path("598830 Shawn Wasabi - Marble Soda/Marble Soda.wav"));
	parser = new OsuParser(song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Normal].osu"));

	OsuBeatmap beatmap = parser->parse();
	beatlist = new BeatList(beatmap);
	if (!m_background_music) {
		printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
		// this might be a critical error...
	}

	m_player_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
	m_player_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));

	if (m_background_music == nullptr || m_player_dead_sound == nullptr || m_player_eat_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
		return false;
	}

	fprintf(stderr, "Loaded music");

	m_current_speed = 1.f;
	healthbar.set_texture(m_textures["healthbar"]);
	healthbar.init();
	bbox hp_bbox = healthbar.get_bounding_box();
	healthbar.set_scale({ 1.0,1.5 });
	healthbar.set_position({ (hp_bbox.max_x - hp_bbox.min_x)/2.0f,200 });
	healthbar.set_rotation(0);
	m_background.init();

	if (m_player.init() && m_boss.init(375.f, &m_little_enemies)) {
		m_player.set_health(2);
		blue_center_beat_circle.init(false);
		orange_center_beat_circle.init(true);
		CenterBeatCircle::player = &m_player;
		LittleEnemy::player = &m_player;
		return true;
	}
	
	
	return false;
}
// World initialization
bool Level1::init() {
	printf("in level init");
	OsuParser* parser;
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}
	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}
	m_background_music = Mix_LoadMUS(song_path("BlendS/BlendS.wav"));
	parser = new OsuParser(song_path("BlendS/Blend A - Bon Appetit S (Meg) [Easy].osu"));

	OsuBeatmap beatmap = parser->parse();
	beatlist = new BeatList(beatmap);
	if (!m_background_music) {
		printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
		// this might be a critical error...
	}

	m_player_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
	m_player_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));

	if (m_background_music == nullptr || m_player_dead_sound == nullptr || m_player_eat_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
		return false;
	}

	fprintf(stderr, "Loaded music");

	m_current_speed = 1.f;

	m_background.init();
	healthbar.set_texture(m_textures["healthbar"]);
	healthbar.init();
	bbox hp_bbox = healthbar.get_bounding_box();
	healthbar.set_scale({ 1.0,1.5 });
	healthbar.set_position({ (hp_bbox.max_x - hp_bbox.min_x)/2.0f,200 });
	healthbar.set_rotation(0);

	if (!m_player.init()){
		return false;
	}
	m_player.set_health(5);
	if (m_boss.init(250.f, &m_little_enemies)) {
		blue_center_beat_circle.init(false);
		orange_center_beat_circle.init(true);
		CenterBeatCircle::player = &m_player;
		LittleEnemy::player = &m_player;
		
		return true;
	}
	
	return false;
}

// Releases all the associated resources
void Level::destroy()
{
	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_player_dead_sound != nullptr)
		Mix_FreeChunk(m_player_dead_sound);
	if (m_player_eat_sound != nullptr)
		Mix_FreeChunk(m_player_eat_sound);

	Mix_CloseAudio();

	m_player.destroy();
	
	for (auto& bullet : m_bullets)
		bullet.destroy();
	for (auto& beatcircle : m_beatcircles)
		beatcircle.destroy();
	for (auto& enemy : m_little_enemies)
		enemy.destroy();
	orange_center_beat_circle.destroy();
	blue_center_beat_circle.destroy();
	
	m_bullets.clear();
	m_little_enemies.clear();
	m_beatcircles.clear();
}


void Level::handle_beat(float remaining_offset, Beat* curBeat, vec2 screen) {
	remaining_offset -= curBeat->relativeOffset;
	//beatPos++;
	// do beat things
	// spawn thing

	


	// spawn a thing
	//spawn_turtle();
	//Turtle& new_turtle = m_turtles.back();
	//new_turtle.set_position({ ((64.f + (float)curBeat->x) / 640.f)*screen.x, ((48.f + (float)curBeat->y) / 480.f)*screen.y });

	m_player.scale_by(1.3);
	m_boss.on_beat(curBeat, screen, m_textures["enemy0"]);
}

// Update our game world
bool Level::update(float elapsed_ms)
{
	if (!Mix_PlayingMusic()) {
		Mix_PlayMusic(m_background_music, 1);
	}
	else {
		m_current_time += elapsed_ms;
	}

	float remaining_offset = elapsed_ms;

	Beat* curBeat;
	while (beatPos < beatlist->beats.size()) {
		curBeat = &beatlist->beats.at(beatPos);
		//printf("remaining offset %f", remaining_offset);
		float center_radius = 100.0f;  // TODO: use radius of circle around player
		float ms_per_beat = curBeat->duration;

		// We should spawn a beat circle such that when the beat circle gets to the
		// center circle, this event coincides with
		// curBeat->offset <= remaining_offset
		float some_fixed_spawn_distance = 500.0f;
		float beat_spawn_time = 1668.f;
		float speed = some_fixed_spawn_distance / beat_spawn_time;
		if (curBeat->absoluteOffset <= m_current_time + beat_spawn_time && !curBeat->spawned) {
			float delta = m_current_time - curBeat->absoluteOffset + beat_spawn_time;
			float pos = some_fixed_spawn_distance - speed * delta;
			float scale = 1.5 - delta / 1500;
			curBeat->spawned = true;
			spawn_beat_circle(curBeat->dir, pos, speed, scale, curBeat->absoluteOffset);
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
	vec2 player_pos = m_player.get_position();
	bool bad = false;
	vec2 mov_dir;
	vec2 bc_player;
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
			Mix_PlayChannel(-1, m_player_dead_sound, 0);
			printf("Boss hit by bullet\n");
			m_boss.set_health(-bullet_it->get_damage());
			m_bullets.erase(bullet_it);
			if (m_boss.get_health() <= 0) {
				finished = 1;
				new_points += 100;
				return true;
			}
			break;
		}
		if (bullet_it->get_bounding_box().max_y < 0) {
			m_bullets.erase(bullet_it);
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
	for (auto little_enemy_it = m_little_enemies.begin(); little_enemy_it != m_little_enemies.end();) {
		if (m_player.collides_with(*little_enemy_it)) {
			little_enemy_it = m_little_enemies.erase(little_enemy_it);
			m_player.set_health(-1);
			printf("%f\n", m_player.get_health());
			float percent_health = m_player.get_health()/5.0f;
			healthbar.set_scale({percent_health, 1.5f});
			bbox hp_bb = healthbar.get_bounding_box();
			healthbar.set_position({(hp_bb.max_x-hp_bb.min_x)/2.0f,200.0f});
			float r = 1.0f;
			//std::min(0.5f+(1.0f-(percent_health))/2.0f, 1.0f);
			float g = std::max(percent_health,0.0f);
			healthbar.set_color(r*2.0f,g,g);
			if (m_player.get_health() <= 0) {
				m_player.kill();
			}
			break;
		} else {
			++little_enemy_it;
		}
	}


	if (m_bullets.size() > 0 && m_little_enemies.size() > 0) {
		for (auto bullet_it = m_bullets.begin(); bullet_it != m_bullets.end();) {
			bool removed_enemy = false;

			for (auto little_enemy_it = m_little_enemies.begin(); little_enemy_it != m_little_enemies.end();) {
				if (little_enemy_it->collides_with(*bullet_it)) {
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
			if (!removed_enemy) { ++bullet_it; };
		}
	}

	return true;
}

// Render our game world
void Level::draw()
{
	int w = screen.x;
	int h = screen.y;
	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5;
	float top = 0.f;// (float)h * -0.5;
	float right = (float)w;// *0.5;
	float bottom = (float)h;// *0.5;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	m_background.set_position({ (float)w / 2, (float)h / 2 });
	
	m_background.draw(projection_2D);
	
	// Drawing entities

	for (auto& bullet : m_bullets)
		bullet.draw(projection_2D);
	for (auto& beatcircle : m_beatcircles)
		beatcircle.draw(projection_2D);
	
	for (auto& enemy : m_little_enemies)
		enemy.draw(projection_2D); 
	m_boss.draw(projection_2D);
	healthbar.draw(projection_2D);
	orange_center_beat_circle.draw(projection_2D);
	blue_center_beat_circle.draw(projection_2D);
	m_player.draw(projection_2D);
}

// Should the game be over ?
bool Level::is_over()const
{
	// TODO: Implement Me
	return finished;
}

// Creates a new turtle and if successfull adds it to the list of turtles


// Creates a new fish and if successfull adds it to the list of fish
bool Level::spawn_bullet(vec2 position, float angle, vec2 scale, bool bullet_type, float dmg, float spd)
{
	Bullet bullet;
	bullet.set_texture(m_textures[bullet_type ? "bullet_1" : "bullet_2"]);
	if (bullet.init(dmg, spd))
	{
		
		bullet.set_position(position);
		bullet.set_rotation(angle);
		bullet.set_scale(scale);
		bullet.m_movement_dir = { (float)cos(angle), (float)-sin(angle) };
		m_bullets.emplace_back(bullet);
		m_player.bullet_type = !bullet_type;

		return true;
	}
	fprintf(stderr, "Failed to spawn fish");
	return false;
}
/*
bool Level::spawn_enemy(vec2 position)
{
	Enemy enemy;
	if (enemy.init())
	{
		enemy.set_position(position);
	
		
		m_enemies.emplace_back(enemy);

		return true;
	}
	fprintf(stderr, "Failed to spawn enemy");
	return false;
}
*/

bool Level::spawn_little_enemy() {
	LittleEnemy littleEnemy;
	littleEnemy.set_texture(m_textures["enemy0"]);
	if (littleEnemy.init()) {
		m_little_enemies.emplace_back(littleEnemy);
		return true;
	}
	fprintf(stderr, "Failed to spawn little enemy");
	return false;
}

bool Level::spawn_beat_circle(int dir, float pos, float speed, float scale, float abs_offset) {
	BeatCircle beat_circle(&m_player, speed, abs_offset);
    bool type = ((dir % 2) == 1);
    beat_circle.set_texture(m_textures[type ? "orange_moving_beat" : "blue_moving_beat"]);
    if (beat_circle.init()) {
        beat_circle.set_dir(dir, pos);
        float angle = m_player.get_rotation();
        beat_circle.set_scale({scale, scale});
        m_beatcircles.emplace_back(beat_circle);
        return true;
    }
    fprintf(stderr, "Failed to spawn beat circle");
    return false;
}


// On key callback
void Level::on_key(int key, int action, int mod)
{
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
			m_player.add_movement_dir({ 1.f, 0.f });
			break;
		case GLFW_KEY_A:
			m_player.add_movement_dir({ -1.f, 0.f });
			break;
		case GLFW_KEY_LEFT_SHIFT:
			m_player.dash();
			break;
		case GLFW_KEY_U:
			m_player.exploding_timer = 1;
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
			m_player.add_movement_dir({ -1.f, 0.f });
			break;
		case GLFW_KEY_A:
			m_player.add_movement_dir({ 1.f, 0.f });
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
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
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

void Level::on_arrow_key(Dir dir)
{
	float player_angle = m_player.get_rotation() + 1.57;
	vec2 salmon_pos = m_player.get_position();
	auto beatcircle_it = m_beatcircles.begin();
	while (beatcircle_it != m_beatcircles.end()) {
		float abs_offset = beatcircle_it->get_offset();
		float delta = abs(m_current_time - abs_offset);
		if (delta > bad_timing) {
			break;
		}

		if (dir != beatcircle_it->get_dir()) {
			beatcircle_it++;
			continue;
		}

		if (delta <= perfect_timing) {
			printf("PERFECT with delta %f\n", delta);
			spawn_bullet(salmon_pos, player_angle, { 0.5, 0.5 }, m_player.bullet_type, 10.f, 5000.f);
			m_beatcircles.erase(beatcircle_it);
			break;
		}
		else if (delta <= good_timing) {
			printf("GOOD with delta %f\n", delta);
			spawn_bullet(salmon_pos, player_angle, { 0.5, 0.5 }, m_player.bullet_type, 5.f, 1500.f);
			m_beatcircles.erase(beatcircle_it);
			break;
		}
		else if (delta <= bad_timing) {
			printf("BAD with delta %f\n", delta);
			spawn_bullet(salmon_pos, player_angle, { 1.f, 1.f }, m_player.bullet_type, 2.f, 800.f);
			m_beatcircles.erase(beatcircle_it);
			break;
		}
		beatcircle_it++;
	}
}

void Level::on_mouse_move(double xpos, double ypos)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	m_player.set_mouse((float)xpos, (float)ypos);
}

void Level::load_textures() {
  std::vector<std::string> texture_names{
    "character",
	"boss0",
    "bullet_1",
    "bullet_2",
    "orange_moving_beat",
    "blue_moving_beat",
	"healthbar",
	"enemy0"
  };

  for (const auto& texture_name : texture_names)
  {
    Texture* texture = new Texture(); 
    // TODO: fix the macro
    auto texture_path = textures_path("") + texture_name + ".png";
    std::cout << texture_path << std::endl;
    if (!texture->load_from_file(texture_path.c_str()))
    {
      fprintf(stderr, "Failed to load texture!");
    }
    m_textures[texture_name] = texture;
  }
}

int Level::getBossHealth() {
	return m_boss.get_health();
}
