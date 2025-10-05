#pragma once

#include <cstddef>

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <core/cameras/CameraBase.h>

#define INIT_SCREEN_WIDTH 1920
#define INIT_SCREEN_HEIGHT 1080

namespace Platform {

class Window
{
public:
    Window(const char* name, unsigned int width, unsigned int height) {
        InitSDL();
        window = SDL_CreateWindow(name, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        InitContext();
        InitGLAD();
        InitOptions();
    };

    ~Window() {
        SDL_GL_DestroyContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
    };
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void swap() {
        SDL_GL_SwapWindow(window);
    };

private:
    SDL_Window* window;
    SDL_GLContext glcontext;

    void InitOptions() {
        SDL_GL_SetSwapInterval(1);
        SDL_SetWindowRelativeMouseMode(window, true); // This causes the cursor to disappear on debug mode, and it doesn't look like there's an easy way to solve this problem.
    }

    void InitSDL() {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    };


    int InitGLAD() {
        if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
            SDL_Log("Failed to initialize GLAD");
            return -1;
        }
        return 0;
    }

    int InitContext() {
        glcontext = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, glcontext);

        return 0;
    }

};

};
