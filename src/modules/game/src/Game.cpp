#include <Game.hpp>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <iostream>

namespace game {

Game::Game() {
	create_window();
};

int Game::create_window() {
    // 1. Initialize SDL3 Video Subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Set OpenGL profile and version (e.g., Core 4.6)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // 3. Create the Window with OpenGL Support
    window = SDL_CreateWindow("SDL3 + GLAD Window", 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Window Creation Failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 4. Create the OpenGL Context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Context Creation Failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 5. Initialize GLAD using SDL's function loader
    // This bridges SDL3 and GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

	while (!done) {
		process_events();
		runtime.render_to_window(600, 600);
		SDL_GL_SwapWindow(window);

	};

    // Cleanup
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
};

void Game::process_events() {
	SDL_Event e;

	runtime.begin_input_frame();

	while (SDL_PollEvent(&e)) {
	//ImGui_ImplSDL3_ProcessEvent(&e);

	if (e.type == SDL_EVENT_QUIT) done = true;

	if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_ESCAPE) {
		//app.client.set_input_mode(client::InputMode::Editor);
		SDL_SetWindowRelativeMouseMode(window, false);
		SDL_CaptureMouse(false);
		SDL_SetWindowMouseGrab(window, false);
	}

	runtime.process_input_event(e);
	}
};

};
