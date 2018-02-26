// Header
#include "level.hpp"
#include "common.hpp"
#include "OsuParser.hpp"
#include "BeatCircle.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>

Texture Level::background_texture;

CenterBeatCircle Level::blue_center_beat_circle;
CenterBeatCircle Level::orange_center_beat_circle;
Player Level::m_player;
unsigned int Level::m_points;
//C++_rng
std::default_random_engine Level::m_rng;
std::uniform_real_distribution<float> Level::m_dist;//default:0..1


// Same as static in c, local to compilation unit
namespace
{
	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

Level::Level(int width, int height)  {
	screen.x = width;
	screen.y = height;
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
	parser = new OsuParser(song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Insane].osu"));

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

	BeatCircle::player = &m_player;

	if (m_player.init()) {
		blue_center_beat_circle.init(false);
		orange_center_beat_circle.init(true);
		CenterBeatCircle::player = &m_player;
		
		return true;
	}
	
	return false;
}
// World initialization
bool Level1::init() {
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

	BeatCircle::player = &m_player;

	if (m_player.init()) {
		blue_center_beat_circle.init(false);
		orange_center_beat_circle.init(true);
		CenterBeatCircle::player = &m_player;
		
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
	orange_center_beat_circle.destroy();
	blue_center_beat_circle.destroy();
	
	m_bullets.clear();

	m_beatcircles.clear();
}


void Level::handle_beat(float remaining_offset, Beat* curBeat, vec2 screen) {
	remaining_offset -= curBeat->offset;
	beatPos++;
	// do beat things
	// spawn thing

	


	// spawn a thing
	//spawn_turtle();
	//Turtle& new_turtle = m_turtles.back();
	//new_turtle.set_position({ ((64.f + (float)curBeat->x) / 640.f)*screen.x, ((48.f + (float)curBeat->y) / 480.f)*screen.y });

	m_player.scale_by(1.3);
}

// Update our game world
bool Level::update(float elapsed_ms)
{
	if (!Mix_PlayingMusic()) {
		Mix_PlayMusic(m_background_music, 1);
	}

	float remaining_offset = elapsed_ms;

	Beat* curBeat;
	while (beatPos < beatlist->beats.size()) {
		curBeat = &beatlist->beats.at(beatPos);
		//printf("remaining offset %f", remaining_offset);
		float center_radius = 100.0f;  // TODO: use radius of circle around player
		float ms_per_beat = curBeat->duration;
		float speed = center_radius / ms_per_beat;
		int dir = 0; // 1 = L, 2 = U, 3 = R, 4 = D
		float pX = ((64.f + (float)curBeat->x) / 640.f);
		float pY = ((48.f + (float)curBeat->y) / 480.f);
		float dA = pX - pY;
		float dB = -1 * (pX + pY) + 1;
		if (dA == 0 && dB == 0)
			dir = 1;
		else if (dA < 0 && dB >= 0)
			dir = 1;
		else if (dA >= 0 && dB >= 0)
			dir = 2;
		else if (dA >= 0 && dB < 0)
			dir = 3;
		else if (dA < 0 && dB < 0)
			dir = 4;

		// We should spawn a beat circle such that when the beat circle gets to the
		// center circle, this event coincides with
		// curBeat->offset <= remaining_offset
		float some_fixed_spawn_distance = 300.0f;
		float beat_spawn_time = speed / some_fixed_spawn_distance;
		if (curBeat->offset - remaining_offset <= beat_spawn_time) {
			float pos = some_fixed_spawn_distance;
			spawn_beat_circle(dir, pos, speed);
		}
		// time_until_next_beat <= elapsed_ms
		if (curBeat->offset <= remaining_offset) {
			handle_beat(remaining_offset, curBeat, screen);
		}
		else {
			curBeat->offset -= remaining_offset;
			//printf("offset: %f\n", curBeat->offset);
			break;
		}
	}

	// Checking player - beatcircle complete overlaps/overshoots
	auto beatcircle_it = m_beatcircles.begin();
	BeatCircle bc;
	vec2 player_pos = m_player.get_position();
	bool bad = false;
	vec2 mov_dir;
	vec2 bc_player;
	while (beatcircle_it != m_beatcircles.end()) {
		bc = (*beatcircle_it);
		bad = length(bc.get_position()) <= 10;
		if (bad) {
			beatcircle_it = m_beatcircles.erase(beatcircle_it);
		}
		else {
			++beatcircle_it;
		}
	}

	// Updating all entities, making the turtle and fish
	// faster based on current
	m_player.update(elapsed_ms);
	float elapsed_modified_ms = elapsed_ms * m_current_speed;
	
	for (auto& bullet : m_bullets)
		bullet.update(elapsed_modified_ms);
	for (auto& beatcircle : m_beatcircles)
		beatcircle.update(elapsed_modified_ms);

	// Removing out of screen turtles
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
bool Level::spawn_bullet(vec2 position, float angle, bool bullet_type, bool on_beat)
{
	Bullet bullet;
	if (bullet.init(bullet_type))
	{
		bullet.set_position(position);
		bullet.set_rotation(angle);
		if (on_beat) {
			bullet.set_scale({ 2,2 });
		}
		bullet.m_movement_dir = { (float)cos(angle), (float)-sin(angle) };
		m_bullets.emplace_back(bullet);

		return true;
	}
	fprintf(stderr, "Failed to spawn fish");
	return false;
}

bool Level::spawn_beat_circle(int dir, float pos, float speed) {
	BeatCircle beatcircle;
	if (beatcircle.init(speed)) {
		beatcircle.set_dir(dir);
		float angle = m_player.get_rotation();
		vec2 spawn_pos = -1 * pos * beatcircle.m_movement_dir;
		beatcircle.set_position(spawn_pos);
		beatcircle.set_scale({ 1.5,1.5 });
		m_beatcircles.emplace_back(beatcircle);
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


	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		bool on_beat = false;
		if (m_beatcircles.size() > 0) {
			BeatCircle closest = m_beatcircles[0];
			float on_beat_radius = 20;
			switch (closest.dir) {
			case 1:
			case 3:
				on_beat_radius = 50;
				break;
			case 2:
			case 4:
				on_beat_radius = 40;
				break;
			}
			on_beat = length(m_beatcircles[0].get_position()) <= on_beat_radius;
			m_beatcircles.erase(m_beatcircles.begin());
		}

		float player_angle = m_player.get_rotation() + 1.57;
		vec2 salmon_pos = m_player.get_position();
		spawn_bullet(salmon_pos, player_angle, m_player.bullet_type, on_beat);
		m_player.bullet_type = !m_player.bullet_type;

	}

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_H:
			finished = 1;
			break;
		case GLFW_KEY_RIGHT:
		case GLFW_KEY_D:
			m_player.add_movement_dir({ 1.f, 0.f });
			break;
		case GLFW_KEY_LEFT:
		case GLFW_KEY_A:
			m_player.add_movement_dir({ -1.f, 0.f });
			break;
			//case GLFW_KEY_UP:
			//case GLFW_KEY_W:
			//	m_player.add_movement_dir({ 0.f, -1.f });
			//	break;
			//case GLFW_KEY_DOWN:
			//case GLFW_KEY_S:
			//	m_player.add_movement_dir({ 0.f, 1.f });
			//	break;
		case GLFW_KEY_LEFT_SHIFT:
			m_player.dash();
			break;

		case GLFW_KEY_U:
			m_player.exploding_timer = 1;
			break;

		case GLFW_KEY_I:
			m_player.player_texture.load_from_file(textures_path("character.png"));
			m_player.exploding_timer = 0;
			m_player.set_scale({ -0.2f, 0.2f });
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_RIGHT:
		case GLFW_KEY_D:
			m_player.add_movement_dir({ -1.f, 0.f });
			break;
		case GLFW_KEY_LEFT:
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

void Level::on_mouse_move(double xpos, double ypos)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	m_player.set_mouse((float)xpos, (float)ypos);
}