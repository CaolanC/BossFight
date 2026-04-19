#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <string>
#include <hv/HttpClient.h>
#include <Client.hpp>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static const char* get_glsl_version() {
#if defined(IMGUI_IMPL_OPENGL_ES2)
    return "#version 100";
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    return "#version 300 es";
#elif defined(__APPLE__)
    return "#version 150";
#else
    return "#version 130";
#endif
}

static void configure_gl_attributes() {
#if defined(IMGUI_IMPL_OPENGL_ES2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

static void apply_theme(float main_scale) {
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    style.WindowRounding = 0.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Button] = ImVec4(90.0f / 255.0f, 90.0f / 255.0f, 102.0f / 255.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(164.0f / 255.0f, 194.0f / 255.0f, 168.0f / 255.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(164.0f / 255.0f, 194.0f / 255.0f, 168.0f / 255.0f, 0.8f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(42.0f / 255.0f, 42.0f / 255.0f, 46.0f / 255.0f, 1.0f);
}

static SDL_Window* create_window(float main_scale) {
    SDL_WindowFlags window_flags =
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_HIDDEN |
        SDL_WINDOW_HIGH_PIXEL_DENSITY;

    return SDL_CreateWindow(
        "Perfect.",
        static_cast<int>(1280 * main_scale),
        static_cast<int>(800 * main_scale),
        window_flags
    );
}

static bool begin_imgui_frame(
    SDL_Window* window,
    SDL_GLContext gl_context,
    bool& done,
    client::Client& client,
    bool viewport_hovered,
    bool viewport_focused
) {
    client.begin_input_frame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT) {
            done = true;
        }

        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
            event.window.windowID == SDL_GetWindowID(window)) {
            done = true;
        }

        if (event.type == SDL_EVENT_KEY_DOWN &&
            event.key.scancode == SDL_SCANCODE_ESCAPE &&
            client.get_input_mode() == client::InputMode::Client) {
            client.set_input_mode(client::InputMode::Editor);
            SDL_SetWindowRelativeMouseMode(window, false);
            continue;
        }

        bool route_to_client = false;

        if (client.get_input_mode() == client::InputMode::Client) {
            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                case SDL_EVENT_MOUSE_MOTION:
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    route_to_client = true;
                    break;

                default:
                    break;
            }
        }

        if (route_to_client) {
            client.process_input_event(event);
        }
    }

    client.end_input_frame();

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
        SDL_Delay(10);
        return false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    return true;
}

static void draw_main_window(
    client::Client& client,
    bool& viewport_hovered,
    bool& viewport_focused,
    bool& viewport_clicked
) {
    viewport_hovered = false;
    viewport_focused = false;
    viewport_clicked = false;

    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    ImGui::Begin(
        "Perfect.",
        nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse
    );

    ImGui::Button("Create Scene", ImVec2(0, 30));
    ImGui::Button("Load Scene", ImVec2(0, 30));
    ImGui::Button("Join Scene", ImVec2(0, 30));

    ImGui::Separator();

    ImGui::BeginChild("Viewport", ImVec2(0, 0), true);

    viewport_hovered = ImGui::IsWindowHovered();
    viewport_focused = ImGui::IsWindowFocused();
    viewport_clicked = viewport_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    ImVec2 avail = ImGui::GetContentRegionAvail();
    int w = static_cast<int>(avail.x);
    int h = static_cast<int>(avail.y);

    if (w > 0 && h > 0) {
        client.update();
        client.render_to_texture(w, h);

        GLuint texture = client.get_render_texture();
        if (texture != 0) {
            ImGui::Image(
                (ImTextureID)(intptr_t)texture,
                ImVec2(static_cast<float>(w), static_cast<float>(h)),
                ImVec2(0, 1),
                ImVec2(1, 0)
            );
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

static void render_imgui(SDL_Window* window, const ImVec4& clear_color) {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    glClearColor(
        clear_color.x * clear_color.w,
        clear_color.y * clear_color.w,
        clear_color.z * clear_color.w,
        clear_color.w
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    configure_gl_attributes();

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window* window = create_window(main_scale);
    if (window == nullptr) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, 1920, 1080);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    apply_theme(main_scale);

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(get_glsl_version());

    client::Client client(
        "Perfect Client.",
        "http://127.0.0.1:30000",
        true,
        false,
        0,
        false,
        client::InputMode::Editor
    );
    client.init_embedded();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool done = false;
    bool viewport_hovered = false;
    bool viewport_focused = false;
    bool viewport_clicked = false;

#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        if (!begin_imgui_frame(window, gl_context, done, client, viewport_hovered, viewport_focused)) {
            continue;
        }

        draw_main_window(client, viewport_hovered, viewport_focused, viewport_clicked);

        if (viewport_clicked) {
            client.set_input_mode(client::InputMode::Client);
            SDL_SetWindowRelativeMouseMode(window, true);
        }

        render_imgui(window, clear_color);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}