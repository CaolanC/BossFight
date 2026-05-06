#include <EditorApp.hpp>
#include <EditorPanels.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <cstring>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <Client.hpp>
#include <Editor.hpp>
#include <SerializedObject.hpp>
#include <LoadedModelInfo.hpp>

namespace gui {

static const char* get_glsl_version() {
    return "#version 130";
}

static bool init(AppContext& app) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    app.window = SDL_CreateWindow("Perfect.", 1280, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);;
    app.gl_context = SDL_GL_CreateContext(app.window);

    SDL_GL_MakeCurrent(app.window, app.gl_context);

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(app.window, app.gl_context);
    ImGui_ImplOpenGL3_Init(get_glsl_version());

    app.client.init_embedded();

    return true;
}

static void shutdown(AppContext& app) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(app.gl_context);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}

    static void lock_dock_node(ImGuiID node_id) {
    if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(node_id)) {
        node->LocalFlags |=
            ImGuiDockNodeFlags_NoDockingSplit |
            ImGuiDockNodeFlags_NoUndocking |
            ImGuiDockNodeFlags_NoTabBar |
            ImGuiDockNodeFlags_NoResize;
        node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        node->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar;
    }
}

    static void build_dock(AppContext& app) {
    if (app.dock_built) return;

    ImGuiID dockspace_id = ImGui::GetID("DockSpace");

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

    ImGuiID right = 0;
    ImGuiID main_left = 0;

    // 1. Split off the right panel first so it spans full height
    ImGui::DockBuilderSplitNode(
        dockspace_id,
        ImGuiDir_Right,
        0.20f,
        &right,
        &main_left
    );

    ImGuiID bottom = 0;
    ImGuiID top_left = 0;

    // 2. Split the remaining left side into bottom + top
    ImGui::DockBuilderSplitNode(
        main_left,
        ImGuiDir_Down,
        0.25f,
        &bottom,
        &top_left
    );

    ImGuiID left = 0;
    ImGuiID center = 0;

    // 3. Split the top-left area into tools + viewport
    ImGui::DockBuilderSplitNode(
        top_left,
        ImGuiDir_Left,
        0.18f,
        &left,
        &center
    );

    ImGui::DockBuilderDockWindow("Tools", left);
    ImGui::DockBuilderDockWindow("Viewport", center);
    ImGui::DockBuilderDockWindow("RightPanel", right);
    ImGui::DockBuilderDockWindow("BottomPanel", bottom);

    ImGui::DockBuilderFinish(dockspace_id);

    lock_dock_node(dockspace_id);
    lock_dock_node(right);
    lock_dock_node(main_left);
    lock_dock_node(bottom);
    lock_dock_node(top_left);
    lock_dock_node(left);
    lock_dock_node(center);

    app.dock_built = true;
}

static void process_events(AppContext& app) {
    SDL_Event e;
    if (app.client.is_scene_ready()) {
        app.client.begin_input_frame();
    }

    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL3_ProcessEvent(&e);

        if (e.type == SDL_EVENT_QUIT)
            app.done = true;

        if (e.type == SDL_EVENT_KEY_DOWN &&
            e.key.scancode == SDL_SCANCODE_ESCAPE) {
            app.client.set_input_mode(client::InputMode::Editor);
            SDL_SetWindowRelativeMouseMode(app.window, false);
            SDL_CaptureMouse(false);
            SDL_SetWindowMouseGrab(app.window, false);
        }

        if (app.client.is_scene_ready() && app.client.get_input_mode() == client::InputMode::Client) {
            app.client.process_input_event(e);
        }
    }

    if (app.client.is_scene_ready()) {
        app.client.end_input_frame();
    }
}

static void draw_dockspace(AppContext& app) {
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);

    ImGui::Begin("DockHost", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBackground);

    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGuiDockNodeFlags dockspace_flags =
    ImGuiDockNodeFlags_NoDockingSplit |
    ImGuiDockNodeFlags_NoUndocking |
    ImGuiDockNodeFlags_NoTabBar |
    ImGuiDockNodeFlags_NoResize;
    ImGui::DockSpace(dockspace_id);

    build_dock(app);

    ImGui::End();
}

static void render(AppContext& app, EditorPanels& panels) {
    if (app.client.isDone()) {
        app.done = true;
    }
    process_events(app);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    draw_dockspace(app);
    panels.draw_tools(app);
    panels.draw_viewport(app);
    panels.draw_right(app);
    panels.draw_bottom(app);

    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(app.clear_color.x, app.clear_color.y, app.clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        SDL_Window* backup = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_ctx = SDL_GL_GetCurrentContext();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        SDL_GL_MakeCurrent(backup, backup_ctx);
    }

    SDL_GL_SwapWindow(app.window);
}

void Editor::run() {
    AppContext app;
    EditorPanels panels;
    if (!init(app)) return;

    while (!app.done) {
        render(app, panels);
    }

    shutdown(app);
}

}