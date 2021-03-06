// internal
#include "common.hpp"
#include "World.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>
#include <stdio.h>
//#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>
#include "DiscordRPC.hpp"

using Clock = std::chrono::high_resolution_clock;

// Global 
World world;
int width = 1600;
int height = 900;
const char* title = "Your Title Here";

// Entry point
int main(int argc, char* argv[])
{

	// Initializing world (after renderer.init().. sorry)
	//printf("HELLO");
	if (argc>1)
		printf(argv[1]);
	bool full_screen_flag = argc>1 && strcmp(argv[1],"-F")==0;
	if (!world.init({ (float)width, (float)height },full_screen_flag))
	{
		// Time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	DiscordRPC::InitDiscord();

	auto t = Clock::now();

	// variable timestep loop.. can be improved (:
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		
		world.update(elapsed_sec);
		world.draw();

		DiscordRPC::RunCallbacks();
	}

	world.destroy();    

	DiscordRPC::ShutdownDiscord();

	return EXIT_SUCCESS;
}
