#pragma once

#include <Runtime.hpp>

namespace game {

class Game {
	public:
	Game();
	private:
	runtime::Runtime runtime;
	void process_events();
	int create_window();
	bool done = false;
	SDL_Window* window;
};

};
