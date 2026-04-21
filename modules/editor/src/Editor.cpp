#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <Client.hpp>
#include <Editor.hpp>

namespace gui {

struct AppContext {
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;

    client::Client client{
        "Perfect Client.",
        "http://127.0.0.1:30000",
        true,
        true,
        0,
        client::InputMode::Editor
    };

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    bool done = false;
    bool viewport_clicked = false;
    bool dock_built = false;

    char ip_input[64] = "127.0.0.1";

    enum class SessionFlowMode {
        None,
        Host,
        Join
    };

    enum class HostSceneMode {
        None,
        Blank,
        FromFile
    };

    SessionFlowMode flow_mode = SessionFlowMode::None;
    HostSceneMode host_scene_mode = HostSceneMode::None;

    char port_input[64] = "30001";
    char file_input[260] = "scene.json";

    bool host_started = false;
    bool guest_started = false;

    bool launch_requested = false;
    std::string status_text = "Idle";
};

static const char* get_glsl_version() {
    return "#version 130";
}

static bool init(AppContext& app) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    app.window = SDL_CreateWindow("Perfect.", 1280, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    // hide_dock_tab_bar(dockspace_id);
    // hide_dock_tab_bar(right);
    // hide_dock_tab_bar(main_left);
    // hide_dock_tab_bar(bottom);
    // hide_dock_tab_bar(top_left);
    // hide_dock_tab_bar(left);
    // hide_dock_tab_bar(center);

    app.dock_built = true;
}

static void process_events(AppContext& app) {
    SDL_Event e;
    app.client.begin_input_frame();

    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL3_ProcessEvent(&e);

        if (e.type == SDL_EVENT_QUIT)
            app.done = true;

        if (e.type == SDL_EVENT_KEY_DOWN &&
            e.key.scancode == SDL_SCANCODE_ESCAPE) {
            app.client.set_input_mode(client::InputMode::Editor);
            SDL_SetWindowRelativeMouseMode(app.window, false);
        }

        if (app.client.get_input_mode() == client::InputMode::Client) {
            app.client.process_input_event(e);
        }
    }

    app.client.end_input_frame();
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

static void draw_tools(AppContext& app) {
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Session");
    ImGui::Separator();

    if (ImGui::Button("Host Session", ImVec2(-1, 30))) {
        app.flow_mode = AppContext::SessionFlowMode::Host;
        app.host_scene_mode = AppContext::HostSceneMode::None;
        app.status_text = "Hosting session";
    }
    if (ImGui::Button("Join Session", ImVec2(-1, 30))){
        app.flow_mode = AppContext::SessionFlowMode::Join;
        app.host_scene_mode = AppContext::HostSceneMode::None;
        app.status_text = "Joining session";
    }

    ImGui::Separator();

    // Host flow

    if (app.flow_mode == AppContext::SessionFlowMode::Host) {
        ImGui::Text("Host Session Setup");
        ImGui::Spacing();

        if (ImGui::Button("New Scene", ImVec2(-1, 28))) {
            app.host_scene_mode = AppContext::HostSceneMode::Blank;
            app.status_text = "Blank scene selected";
        }

        if (ImGui::Button("Load From File", ImVec2(-1, 28))) {
            app.host_scene_mode = AppContext::HostSceneMode::FromFile;
            app.status_text = "Load-from-file selected";
        }

        ImGui::Spacing;

        if (app.host_scene_mode == AppContext::HostSceneMode::Blank) {
            ImGui::TextWrapped("A blank scene will be created when host flow is hooked up");

            if (ImGui::Button("Start Host Session", ImVec2(-1, 30))) {
                // Put actual hookup in client
                app.client.setIsHost(true);
                app.status_text = "Host is set to true, load blank scene";
            }

        }
        else if (app.host_scene_mode == AppContext::HostSceneMode::FromFile) {
            ImGui::Text("Scene file");
            ImGui::InputText("##scene_file", app.file_input, sizeof(app.file_input));

            if (ImGui::Button("Start Host Session", ImVec2(-1, 30))) {
                app.client.setIsHost(true);
                app.status_text = "Host is set to true on loading scene from file";
            }
        }
    }

    // Joining

    else if (app.flow_mode == AppContext::SessionFlowMode::Join) {
        ImGui::Text("Join Session");
        ImGui::Spacing();

        ImGui::Text("Server IP");
        ImGui::InputText("##ip", app.ip_input, sizeof(app.ip_input));

        ImGui::Text("Port");
        ImGui::InputText("##port", app.port_input, sizeof(app.port_input));

        if (ImGui::Button("Join", ImVec2(-1, 30))) {
            // GUI-only for now.
            app.client.setIsHost(false);
            app.status_text =
                std::string("TODO: join ") + app.ip_input + ":" + app.port_input;
        }
    }

    ImGui::Separator();
    ImGui::TextWrapped("Status: %s", app.status_text.c_str());

    ImGui::End();
}

static void draw_viewport(AppContext& app) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);

    ImVec2 size = ImGui::GetContentRegionAvail();

    int w = (int)size.x;
    int h = (int)size.y;

    if (w > 0 && h > 0) {
        app.client.update();
        app.client.render_to_texture(w, h);

        GLuint tex = app.client.get_render_texture();

        ImGui::Image((ImTextureID)(intptr_t)tex, size, ImVec2(0,1), ImVec2(1,0));
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
        app.client.set_input_mode(client::InputMode::Client);
        SDL_SetWindowRelativeMouseMode(app.window, true);
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

static void draw_right(AppContext& app) {
    ImGui::Begin("RightPanel", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::Separator();

    const char* flow = "None";
    if (app.flow_mode == AppContext::SessionFlowMode::Host) flow = "Host";
    else if (app.flow_mode == AppContext::SessionFlowMode::Join) flow = "Join";

    const char* host_scene = "None";
    if (app.host_scene_mode == AppContext::HostSceneMode::Blank) host_scene = "Blank";
    else if (app.host_scene_mode == AppContext::HostSceneMode::FromFile) host_scene = "FromFile";

    ImGui::Text("Flow Mode: %s", flow);
    ImGui::Text("Host Scene Mode: %s", host_scene);
    ImGui::TextWrapped("Server IP: %s", app.ip_input);
    ImGui::TextWrapped("Port: %s", app.port_input);
    ImGui::TextWrapped("Scene File: %s", app.file_input);

    ImGui::End();
}

static void draw_bottom() {
    ImGui::Begin("BottomPanel", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Console");
    ImGui::End();
}

static void render(AppContext& app) {
    process_events(app);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    draw_dockspace(app);
    draw_tools(app);
    draw_viewport(app);
    draw_right(app);
    draw_bottom();

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
    if (!init(app)) return;

    while (!app.done) {
        render(app);
    }

    shutdown(app);
}

}