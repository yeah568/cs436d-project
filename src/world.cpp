// Header
#include "world.hpp"
#include "common.hpp"
#include "OsuParser.hpp"
#include "BeatCircle.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>

// Same as static in c, local to compilation unit
namespace
{
	const size_t MAX_TURTLES = 15;
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

World::World() :
	m_points(0),
	//m_next_turtle_spawn(0.f),
	m_next_fish_spawn(0.f)

{
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{

}

// World initialization
bool World::init(vec2 screen)
{
	printf("End of world");
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "A1 Assignment", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	//-------------------------------------------------------------------------
	OsuParser* parser = new OsuParser(song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Normal].osu"));
	OsuBeatmap beatmap = parser->parse();

	beatlist = new BeatList(beatmap);

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	//int result = 0;
	//int flags = MIX_INIT_MP3;

	//if (flags != (result = Mix_Init(flags))) {
	//	printf("Could not initialize mixer (result: %d).\n", result);
	//	printf("Mix_Init: %s\n", Mix_GetError());
	//	return false;
	//}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	//m_background_music = Mix_LoadMUS(audio_path("music.wav"));
	m_background_music = Mix_LoadMUS(song_path("598830 Shawn Wasabi - Marble Soda/Marble Soda.wav"));

	if (!m_background_music) {
		printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
		// this might be a critical error...
	}

	m_salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
	m_salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));

	if (m_background_music == nullptr || m_salmon_dead_sound == nullptr || m_salmon_eat_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
		return false;
	}

	fprintf(stderr, "Loaded music");

	m_current_speed = 1.f;

	m_background.init();

	BeatCircle::player = &m_salmon;
	
	if (m_salmon.init()) {
		blue_center_beat_circle.init(false);
		orange_center_beat_circle.init(true);
		CenterBeatCircle::player = &m_salmon;
		printf("End of world");
		return true;
	}
	printf("End of world");
	return false;
}

// Releases all the associated resources
void World::destroy()
{
	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_salmon_dead_sound != nullptr)
		Mix_FreeChunk(m_salmon_dead_sound);
	if (m_salmon_eat_sound != nullptr)
		Mix_FreeChunk(m_salmon_eat_sound);

	Mix_CloseAudio();

	m_salmon.destroy();
	for (auto& turtle : m_turtles)
		turtle.destroy();
	for (auto& bullet : m_bullets)
		bullet.destroy();
	for (auto& beatcircle : m_beatcircles)
		beatcircle.destroy();
	orange_center_beat_circle.destroy();
	blue_center_beat_circle.destroy();
	m_turtles.clear();
	m_bullets.clear();
	
	m_beatcircles.clear();
	glfwDestroyWindow(m_window);
}


void World::handle_beat(float remaining_offset, Beat* curBeat, vec2 screen) {
	remaining_offset -= curBeat->offset;
	beatPos++;
	// do beat things
	// spawn thing

	printf("spawn %f\n", curBeat->offset);

	
	// spawn a thing
	//spawn_turtle();
	//Turtle& new_turtle = m_turtles.back();
	//new_turtle.set_position({ ((64.f + (float)curBeat->x) / 640.f)*screen.x, ((48.f + (float)curBeat->y) / 480.f)*screen.y });

	m_salmon.scale_by(1.3);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	if (!Mix_PlayingMusic()) {
		Mix_PlayMusic(m_background_music, 1);
	}

	float remaining_offset = elapsed_ms;

	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
	float screen_scale = getWindowRatio();
	w = w / screen_scale;
	h = h/screen_scale;
	vec2 screen = { (float)w, (float)h };

	Beat* curBeat;
	while (beatPos < beatlist->beats.size()) {
		curBeat = &beatlist->beats.at(beatPos);
		//printf("remaining offset %f", remaining_offset);
		float center_radius = 100.0f;  // TODO: use radius of circle around player
		float ms_per_beat = curBeat->duration;
		float speed = center_radius/ms_per_beat;
		int dir = 0; // 1 = L, 2 = U, 3 = R, 4 = D
		float pX = ((64.f + (float)curBeat->x) / 640.f);
		float pY = ((48.f + (float)curBeat->y) / 480.f);
		float dA = pX - pY;
		float dB = -1*(pX + pY) + 1;
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
		float beat_spawn_time = speed/some_fixed_spawn_distance;
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
	vec2 player_pos = m_salmon.get_position();
	bool bad = false;
	vec2 mov_dir;
	vec2 bc_player;
	while (beatcircle_it != m_beatcircles.end()) {
		bc = (*beatcircle_it);
		bad = length(bc.get_position()) <= 10;
		if (bad) {
			beatcircle_it = m_beatcircles.erase(beatcircle_it);
		} else {
			++beatcircle_it;
		}
	}
	


	// Checking Salmon - Turtle collisions
	// for (const auto& turtle : m_turtles)
	// {
	// 	if (m_salmon.collid es_with(turtle))
	// 	{
	// 		if (m_salmon.is_alive())
	// 			Mix_PlayChannel(-1, m_salmon_dead_sound, 0);
	// 		m_salmon.kill();
	// 		break;
	// 	}
	// }

	// Checking Salmon - Fish collisions
	//auto fish_it = m_fish.begin();
	/*
	while (fish_it != m_fish.end())
	{
		if (m_salmon.collides_with(*fish_it))
		{
			fish_it = m_fish.erase(fish_it);
			m_salmon.light_up();
			Mix_PlayChannel(-1, m_salmon_eat_sound, 0);
			++m_points;
		}
		else
			++fish_it;
	}
	*/

	// Updating all entities, making the turtle and fish
	// faster based on current
	m_salmon.update(elapsed_ms);
	float elapsed_modified_ms = elapsed_ms * m_current_speed;
	for (auto& turtle : m_turtles)
		turtle.update(elapsed_modified_ms);
	for (auto& bullet : m_bullets)
		bullet.update(elapsed_modified_ms);
	for (auto& beatcircle : m_beatcircles)
		beatcircle.update(elapsed_modified_ms);

	// Removing out of screen turtles
	auto turtle_it = m_turtles.begin();
	while (turtle_it != m_turtles.end())
	{
		float w = turtle_it->get_bounding_box().x / 2;
		if (turtle_it->get_position().x + w < 0.f)
		{
			turtle_it = m_turtles.erase(turtle_it);
			continue;
		}

		++turtle_it;
	}

	// Removing out of screen fish
	//fish_it = m_fish.begin();
	//while (fish_it != m_fish.end())
	//{
	//	float w = fish_it->get_bounding_box().x / 2;
	//	if (fish_it->get_position().x + w < 0.f)
	//	{
	//		fish_it = m_fish.erase(fish_it);
	//		continue;
	//	}

	//	++fish_it;
	//}

	// TODO: remove out of screen bullets

	// Spawning new turtles
	/*
	m_next_turtle_spawn -= elapsed_ms * m_current_speed;
	if (m_turtles.size() <= MAX_TURTLES && m_next_turtle_spawn < 0.f)
	{
		if (!spawn_turtle())
			return false;

		Turtle& new_turtle = m_turtles.back();

		// Setting random initial position
		new_turtle.set_position({ screen.x + 150, 50 + m_dist(m_rng) * (screen.y - 100) });

		// Next spawn
		m_next_turtle_spawn = (TURTLE_DELAY_MS / 2) + m_dist(m_rng) * (TURTLE_DELAY_MS/2);
	}
	*/

	// Spawning new fish
	/*
	m_next_fish_spawn -= elapsed_ms * m_current_speed;
	if (m_fish.size() <= MAX_FISH && m_next_fish_spawn < 0.f)
	{
		if (!spawn_fish())
			return false;
		Fish& new_fish = m_fish.back();

		new_fish.set_position({ screen.x + 150, 50 + m_dist(m_rng) *  (screen.y - 100) });

		m_next_fish_spawn = (FISH_DELAY_MS / 2) + m_dist(m_rng) * (FISH_DELAY_MS / 2);
	}
	*/
	return true;
}

// Render our game world
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
		float screen_scale = getWindowRatio();
		w = w / screen_scale;
		h = h / screen_scale;


	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << m_points;
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	for (auto& turtle : m_turtles)
		turtle.draw(projection_2D);
	for (auto& bullet : m_bullets)
		bullet.draw(projection_2D);
	
	for (auto& beatcircle : m_beatcircles)
		beatcircle.draw(projection_2D);
	orange_center_beat_circle.draw(projection_2D);
	blue_center_beat_circle.draw(projection_2D);
	m_salmon.draw(projection_2D);

	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
	return glfwWindowShouldClose(m_window);
}

// Creates a new turtle and if successfull adds it to the list of turtles
bool World::spawn_turtle()
{
	Turtle turtle;
	if (turtle.init())
	{
		m_turtles.emplace_back(turtle);
		return true;
	}
	fprintf(stderr, "Failed to spawn turtle");
	return false;
}

// Creates a new fish and if successfull adds it to the list of fish
bool World::spawn_bullet(vec2 position, float angle,bool bullet_type, bool on_beat)
{
	Bullet bullet;
	if (bullet.init(bullet_type))
	{
		bullet.set_position(position);
		bullet.set_rotation(angle);
		if (on_beat) {
			bullet.set_scale({2,2});
		}
		bullet.m_movement_dir = { (float)cos(angle), (float)-sin(angle) };
		m_bullets.emplace_back(bullet);

		return true;
	}
	fprintf(stderr, "Failed to spawn fish");
	return false;
}

bool World::spawn_beat_circle(int dir, float pos, float speed) {
	BeatCircle beatcircle;
	if (beatcircle.init(speed)) {
		beatcircle.set_dir(dir);
		float angle = m_salmon.get_rotation();
		vec2 spawn_pos = -1*pos * beatcircle.m_movement_dir;
		beatcircle.set_position(spawn_pos);
		beatcircle.set_scale({1.5,1.5});
		m_beatcircles.emplace_back(beatcircle);
		return true;
	}
	fprintf(stderr, "Failed to spawn beat circle");
	return false;
}


// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	float screen_scale = getWindowRatio();
	w = w / screen_scale;
	h = h / screen_scale;
	vec2 screen = { (float)w, (float)h };


	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		bool on_beat =false;
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
		
		float player_angle = m_salmon.get_rotation()+1.57;
		vec2 salmon_pos = m_salmon.get_position();
		spawn_bullet(salmon_pos, player_angle, m_salmon.bullet_type, on_beat);
		m_salmon.bullet_type = !m_salmon.bullet_type;
		
	}

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_RIGHT:
		case GLFW_KEY_D:
			m_salmon.add_movement_dir({ 1.f, 0.f });
			break;
		case GLFW_KEY_LEFT:
		case GLFW_KEY_A:
			m_salmon.add_movement_dir({ -1.f, 0.f });
			break;
		//case GLFW_KEY_UP:
		//case GLFW_KEY_W:
		//	m_salmon.add_movement_dir({ 0.f, -1.f });
		//	break;
		//case GLFW_KEY_DOWN:
		//case GLFW_KEY_S:
		//	m_salmon.add_movement_dir({ 0.f, 1.f });
		//	break;
		case GLFW_KEY_LEFT_SHIFT:
			m_salmon.dash();
			break;

		case GLFW_KEY_U:
			m_salmon.exploding_timer = 1;
			break;

		case GLFW_KEY_I:
			m_salmon.player_texture.load_from_file(textures_path("character.png"));
			m_salmon.exploding_timer = 0;
			m_salmon.set_scale({ -0.2f, 0.2f });
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_RIGHT:
		case GLFW_KEY_D:
			m_salmon.add_movement_dir({ -1.f, 0.f });
			break;
		case GLFW_KEY_LEFT:
		case GLFW_KEY_A:
			m_salmon.add_movement_dir({ 1.f, 0.f });
			break;
		//case GLFW_KEY_UP:
		//case GLFW_KEY_W:
		//	m_salmon.add_movement_dir({ 0.f, 1.f });
		///	break;
		//case GLFW_KEY_DOWN:
		//case GLFW_KEY_S:
		//	m_salmon.add_movement_dir({ 0.f, -1.f });
		//	break;
		}
	}


	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
		m_salmon.destroy();
		m_salmon.init();
		m_background.init();
		m_turtles.clear();
		m_bullets.clear();
		m_current_speed = 1.f;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) &&  key == GLFW_KEY_COMMA)
		m_current_speed -= 0.1f;
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
		m_current_speed += 0.1f;

	m_current_speed = fmax(0.f, m_current_speed);
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	m_salmon.set_mouse((float)xpos, (float)ypos);

}