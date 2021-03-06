#include "World.hpp"
#include "common.hpp"
#include "Level.hpp"
#include "TextureManager.hpp"

#include <string.h>
#include <cassert>
#include <sstream>
#include <stdlib.h>

// Same as static in c, local to compilation unit
namespace
{

    namespace {
        void glfw_err_cb(int error, const char *desc) {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

unsigned int Level::m_score;

World::World()
{	
	m_points = 0;
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World(){}

// World initialization
bool World::init(vec2 screen, bool is_full_screen)
{
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	levelCounter = 0;
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
	vec2 actual_screen;
	if (is_full_screen)
	{
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		m_window = glfwCreateWindow(mode->width, mode->height, "BeatCoin", glfwGetPrimaryMonitor(), nullptr);
		actual_screen = {(float)mode->width, (float)mode->height};
	}
	else
	{
		m_window = glfwCreateWindow(screen.x, screen.y, "BeatCoin", nullptr, nullptr);
		actual_screen = screen;
	}
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
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) {
        ((World *) glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3);
    };
    auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
        ((World *) glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1);
    };
	auto cursor_button_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) {
		((World *)glfwGetWindowUserPointer(wnd))->on_mouse_button(wnd, _0, _1, _2);
	};
	auto scroll_redirect = [](GLFWwindow *wnd, double _0, double _1) {
		((World *)glfwGetWindowUserPointer(wnd))->on_mouse_scroll(wnd, _0, _1);
	};
    glfwSetKeyCallback(m_window, key_redirect);
    glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(m_window, cursor_button_redirect);
	glfwSetScrollCallback(m_window, scroll_redirect);
	TextureManager* tm = TextureManager::get_instance();
	Level::m_score = 0;
	//return levelList[levelCounter].init
	//Level1* level = new Level1(screen.x, screen.y);
	Level::levelList = &levelList;
	levelList.emplace_back(new MainMenu(screen.x, screen.y));
	//levelList.emplace_back(new Level1(screen.x, screen.y));
	//levelList.emplace_back(new Level2(screen.x, screen.y));
	//levelList.emplace_back(new Level1(screen.x, screen.y, 3));
	
	return levelList[levelCounter]->init();
}

// Releases all the associated resources
void World::destroy()
{	
	levelList[levelCounter]->destroy();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	if (levelList[levelCounter]->is_over()) 
	{	
		if (levelCounter == levelList.size()-1) {
			exit(0);
		} else {
			levelList[levelCounter]->destroy();
		}
		levelCounter++;
		levelList[levelCounter]->init();

		// Updating window title with points
		std::stringstream title_ss;
		title_ss << "Beatcoin - " << levelList[levelCounter]->getTitleText();
		glfwSetWindowTitle(m_window, title_ss.str().c_str());
	};
	return levelList[levelCounter]->update(elapsed_ms);
}

// Render our game world
void World::draw()
{

	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	levelList[levelCounter]->draw();

	// Presenting
	glfwSwapBuffers(m_window);
}

// Spawning new fish
/*
m_next_fish_spawn -= elapsed_ms * m_current_speed;
if (m_fish.size() <= MAX_FISH && m_next_fish_spawn < 0.f)
{

	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    m_next_fish_spawn = (FISH_DELAY_MS / 2) + m_dist(m_rng) * (FISH_DELAY_MS / 2);
}
*/

// Should the game be over ?
bool World::is_over() const {
    return glfwWindowShouldClose(m_window);
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	levelList[levelCounter]->on_key(key, action, mod);
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE SALMON ROTATION HERE
    // xpos and ypos are relative to the top-left of the window, the salmon's
    // default facing direction is (1, 0)
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	levelList[levelCounter]->on_mouse_move(xpos, ypos);
}

void World::on_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		printf("Clicking");
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		levelList[levelCounter]->on_mouse_click({(float)xpos, (float)ypos});
	}
}

void World::on_mouse_scroll(GLFWwindow* window, double xoff, double yoff) {
	levelList[levelCounter]->on_mouse_scroll(window, {(float)xoff, (float)yoff});
}