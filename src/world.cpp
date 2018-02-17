// Header
#include "world.hpp"
#include "common.hpp"
#include "OsuParser.hpp"

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
	m_next_turtle_spawn(0.f),
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
	OsuParser* parser = new OsuParser(song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Exa) [Insane].osu"));
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
	
	OsuParser* parser = new OsuParser(song_path("598830 Shawn Wasabi - Marble Soda/Shawn Wasabi - Marble Soda (Stingy) [Oni Zero].osu"));
	parser->parse();

	return m_salmon.init();
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
	for (auto& fish : m_fish)
		fish.destroy();
	for (auto& bullet : m_bullets)
		bullet.destroy();
	m_turtles.clear();
	m_fish.clear();
	m_bullets.clear();
	glfwDestroyWindow(m_window);
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
	vec2 screen = { (float)w, (float)h };

	Beat* curBeat;
	// In each update call, iterate through all beat objects and spawn all things
	// needed up to that point.
	while (beatPos < beatlist->beats.size()) {
		curBeat = &beatlist->beats.at(beatPos);

		// if the time elapsed is less than the delta for the next beat, iterate
		// and subtract the beat offset amount from the remaining_offset.
		if (curBeat->offset <= remaining_offset) {
			remaining_offset -= curBeat->offset;
			beatPos++;

			// TODO jamesliu: replace this "pulsing" with actual desired features
			// TODO jamesliu: try a max_scale or set_scale instead of compounding
			// the scaling
			m_salmon.scale_by(1.3);
		}
		else {
			curBeat->offset -= remaining_offset;
			break;
		}
	}


	// Checking Salmon - Turtle collisions
	for (const auto& turtle : m_turtles)
	{
		if (m_salmon.collides_with(turtle))
		{
			if (m_salmon.is_alive())
				Mix_PlayChannel(-1, m_salmon_dead_sound, 0);
			m_salmon.kill();
			break;
		}
	}

	// Checking Salmon - Fish collisions
	auto fish_it = m_fish.begin();
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
	for (auto& turtle : m_turtles)
		turtle.update(elapsed_ms * m_current_speed);
	for (auto& fish : m_fish)
		fish.update(elapsed_ms * m_current_speed);
	for (auto& bullet : m_bullets)
		bullet.update(elapsed_ms * m_current_speed);

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
	fish_it = m_fish.begin();
	while (fish_it != m_fish.end())
	{
		float w = fish_it->get_bounding_box().x / 2;
		if (fish_it->get_position().x + w < 0.f)
		{
			fish_it = m_fish.erase(fish_it);
			continue;
		}

		++fish_it;
	}

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
	/*
	for (auto& turtle : m_turtles)
		turtle.draw(projection_2D);
	*/
	for (auto& fish : m_fish)
		fish.draw(projection_2D);
	for (auto& bullet : m_bullets)
		bullet.draw(projection_2D);
	
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
bool World::spawn_fish(vec2 position, float angle, bool type)
{
	Fish fish;
	if (fish.init(type))
	{
		fish.set_position(position);
		fish.set_rotation(angle);
		fish.m_movement_dir = { (float)cos(angle), (float)-sin(angle) };
		m_fish.emplace_back(fish);
		
		return true;
	}
	fprintf(stderr, "Failed to spawn fish");
	return false;
}

bool World::spawn_bullet(float angle, vec2 position)
{
	Bullet bullet;
	if (bullet.init(angle, position))
	{
		m_bullets.emplace_back(bullet);
		return true;
	}
	fprintf(stderr, "Failed to spawn bullet");
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
	vec2 screen = { (float)w, (float)h };
	

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		float player_angle = m_salmon.get_rotation();
		vec2 salmon_pos = m_salmon.get_position();
		spawn_fish(salmon_pos, player_angle, m_salmon.bullet_type);
		m_salmon.bullet_type = !m_salmon.bullet_type;
		printf("Salmon bullet type %d", m_salmon.bullet_type);
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
		case GLFW_KEY_UP:
		case GLFW_KEY_W:
			m_salmon.add_movement_dir({ 0.f, -1.f });
			break;
		case GLFW_KEY_DOWN:
		case GLFW_KEY_S:
			m_salmon.add_movement_dir({ 0.f, 1.f });
			break;
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
		case GLFW_KEY_UP:
		case GLFW_KEY_W:
			m_salmon.add_movement_dir({ 0.f, 1.f });
			break;
		case GLFW_KEY_DOWN:
		case GLFW_KEY_S:
			m_salmon.add_movement_dir({ 0.f, -1.f });
			break;
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
		m_fish.clear();
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
