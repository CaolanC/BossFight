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

struct AppContext {
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;

    client::Client client{
        "Perfect Client.",
        "http://127.0.0.1:30000",
        true,
        false,
        0,
        client::InputMode::Editor
    };

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    bool done = false;
    bool viewport_clicked = false;
    bool dock_built = false;

    char guest_ip_input[64] = "http://<serverip>";
    char host_ip_input[64] = "http://<serverip>";

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
    char savetofile_input[260] = "";

    bool host_started = false;
    bool guest_started = false;

    bool launch_requested = false;
    std::string status_text = "Idle";

    std::string selected_object_id;
    core::SerializedObject selected_object{};

    xg::Guid selected_model_ref{};
    std::string selected_model_path;

    char import_model_path[260] = "";
    char objectname[128] = "";

};

static const char* get_glsl_version() {
    return "#version 130";
}

static bool init(AppContext& app) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // new

    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //

    app.window = SDL_CreateWindow("Perfect.", 1280, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);;
    app.gl_context = SDL_GL_CreateContext(app.window);

    //vSDL_GL_SetSwapInterval(1); // new

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

static void draw_tools(AppContext& app) {
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoTitleBar);

    if (!app.client.is_scene_ready()) {
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

            ImGui::InputText("##host_ip_input", app.host_ip_input, sizeof(app.host_ip_input));

            if (ImGui::Button("New Scene", ImVec2(-1, 28))) {
                app.host_scene_mode = AppContext::HostSceneMode::Blank;
                app.status_text = "Blank scene selected";
            }

            if (ImGui::Button("Load From File", ImVec2(-1, 28))) {
                app.host_scene_mode = AppContext::HostSceneMode::FromFile;
                app.status_text = "Load-from-file selected";
            }

            ImGui::Spacing();

            if (app.host_scene_mode == AppContext::HostSceneMode::Blank) {
                ImGui::TextWrapped("A blank scene will be created.");

                if (ImGui::Button("Start Host Session", ImVec2(-1, 30))) {
                    bool ok = app.client.start_host_blank(std::string(app.host_ip_input) + ":30000");
                    app.status_text = ok ? "Host session started (blank scene)" : "Failed to start host session";
                }

            }
            else if (app.host_scene_mode == AppContext::HostSceneMode::FromFile) {
                ImGui::Text("Scene file");
                ImGui::InputText("##scene_file", app.file_input, sizeof(app.file_input));

                if (ImGui::Button("Start Host Session", ImVec2(-1, 30))) {
                    bool ok = app.client.start_host_file(
                        std::string(app.host_ip_input) + ":30000",
                        std::string(app.file_input)
                        );
                    app.status_text = ok? std::string("Host session started from file: ") + app.file_input : "Failed to start host session from file";
                }


            }
        }

        // Joining

        else if (app.flow_mode == AppContext::SessionFlowMode::Join) {
            ImGui::Text("Join Session");
            ImGui::Spacing();

            ImGui::Text("Server IP");
            ImGui::InputText("##ip", app.guest_ip_input, sizeof(app.guest_ip_input));

            ImGui::Text("Port");
            ImGui::InputText("##port", app.port_input, sizeof(app.port_input));

            if (ImGui::Button("Join", ImVec2(-1, 30))) {
                app.client.setIsHost(false);
                int port = std::atoi(app.port_input);
                bool ok = app.client.start_guest(std::string(app.guest_ip_input), port);
                app.status_text = ok? std::string("Joining session at ") + app.guest_ip_input + ":" + app.port_input : "Failed to start guest session";
            }
        }

        ImGui::Separator();
        ImGui::TextWrapped("Status: %s", app.status_text.c_str());
    }
    else if (app.client.is_scene_ready() && app.client.getIsHost()) {
        ImGui::Text("Host Client");
        ImGui::Separator();

        ImGui::InputText("##save_file", app.savetofile_input, sizeof(app.savetofile_input));
        if (ImGui::Button("Save and Quit", ImVec2(-1, 30))) {
            if (app.client.save_and_quit(app.savetofile_input)) {
                app.done = true;
            }
            else {
                app.status_text = "Failed to save to file, client still open.";
            }
        }
    }
    else if (app.client.is_scene_ready() && !(app.client.getIsHost())) {
        ImGui::Text("Guest Client");
    }

    ImGui::End();
}

static void draw_viewport(AppContext& app) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);

    app.client.process_network_messages();

    if (!app.client.is_scene_ready()) {
        ImGui::Dummy(ImVec2(0.0f, 40.0f));
        ImGui::TextWrapped("No active scene yet.");
        ImGui::Spacing();
        ImGui::TextWrapped("Choose one of the options in the Tools panel:");
        ImGui::BulletText("Host Session -> New Scene");
        ImGui::BulletText("Host Session -> Load From File");
        ImGui::BulletText("Join Session -> enter port and join");

        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    ImVec2 size = ImGui::GetContentRegionAvail();
    // ImGuiIO& io = ImGui::GetIO();

    int w = (int)size.x;
    int h = (int)size.y;

    // int w = (int)(size.x * io.DisplayFramebufferScale.x);
    // int h = (int)(size.y * io.DisplayFramebufferScale.y);

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
    ImGui::Text("Scene Objects");
    ImGui::Separator();

    // const char* flow = "None";
    // if (app.flow_mode == AppContext::SessionFlowMode::Host) flow = "Host";
    // else if (app.flow_mode == AppContext::SessionFlowMode::Join) flow = "Join";
    //
    // const char* host_scene = "None";
    // if (app.host_scene_mode == AppContext::HostSceneMode::Blank) host_scene = "Blank";
    // else if (app.host_scene_mode == AppContext::HostSceneMode::FromFile) host_scene = "FromFile";
    //
    // ImGui::Text("Flow Mode: %s", flow);
    // ImGui::Text("Host Scene Mode: %s", host_scene);
    // ImGui::TextWrapped("Server IP: %s", app.ip_input);
    // ImGui::TextWrapped("Port: %s", app.port_input);
    // ImGui::TextWrapped("Scene File: %s", app.file_input);

    if (!app.client.is_scene_ready()) {
        ImGui::TextWrapped("No active scene.");
        ImGui::End();
        return;
    }

    auto objects = app.client.get_scene_objects();

    ImGui::BeginChild("object_list", ImVec2(0, 220), true);

    for (const auto& obj : objects) {
        bool selected = (app.selected_object_id == obj.objectID);
        std::string display_name = obj.name.empty() ? "Object" : obj.name;
        std::string label = display_name + "##" + obj.objectID;

        if (ImGui::Selectable(label.c_str(), selected)) {
            app.selected_object_id = obj.objectID;
            app.selected_object = obj;
        }
    }

    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Selected Object");
    ImGui::Spacing();

    if (app.selected_object_id.empty()) {
        ImGui::TextWrapped("Select an object from the list above.");
        ImGui::End();
        return;
    }

    if (!(app.selected_object_id.empty()) && !(app.client.scene.check_registry(app.selected_object.objectID))) {
        app.selected_object_id = "";
        app.selected_object = core::SerializedObject();
    }

    ImGui::TextWrapped("Object ID: %s", app.selected_object.objectID.c_str());
    ImGui::TextWrapped("Model Path: %s", app.selected_object.model_path.c_str());

    char name[128];
    std::strncpy(name, app.selected_object.name.c_str(), 128);
    name[sizeof(name) - 1] = '\0';

    if (ImGui::InputText("Name", name, sizeof(name))) {
        app.selected_object.name = name;
    }

    float pos[3] = {
        app.selected_object.position.x,
        app.selected_object.position.y,
        app.selected_object.position.z
    };

    float rot[4] = {
        app.selected_object.rotation.x,
        app.selected_object.rotation.y,
        app.selected_object.rotation.z,
        app.selected_object.rotation.w
    };

    float scale = app.selected_object.scale;

    ImGui::Spacing();
    ImGui::Text("Transform");

    bool changed = false;
    changed |= ImGui::DragFloat3("Position", pos, 0.05f);
    changed |= ImGui::DragFloat4("Rotation (quat)", rot, 0.05f);
    changed |= ImGui::DragFloat("Scale", &scale, 0.05f, 0.01f, 100.0f);

    if (changed) {
        app.selected_object.position = glm::vec3(pos[0], pos[1], pos[2]);
        app.selected_object.rotation = glm::quat(rot[3], rot[0], rot[1], rot[2]);
        app.selected_object.scale = scale;
    }

    if (ImGui::Button("Apply Edit", ImVec2(-1, 30))) {
        bool ok = app.client.apply_gui_edit(app.selected_object);
        app.status_text = ok ? "Object edited successfully" : "Failed to edit object";

        if (ok) {
            core::SerializedObject latest;
            if (app.client.get_scene_object(app.selected_object_id, latest)) {
                app.selected_object = latest;
            }
        }
    }

    if (ImGui::Button("Delete Object", ImVec2(-1, 30))) {
        bool ok = app.client.apply_gui_delete(app.selected_object);
        app.status_text = ok ? "Object deleted successfully" : "Failed to delete object";

        if (ok) {
            app.selected_object = core::SerializedObject();
            app.selected_object_id = "";
        }
    }

    ImGui::End();
}

static void draw_bottom(AppContext& app) {
    ImGui::Begin("BottomPanel", nullptr, ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginTabBar("BottomTabs")) {

        if (ImGui::BeginTabItem("Loaded Models")) {
            if (!app.client.is_scene_ready()) {
                ImGui::TextWrapped("No active scene.");
            }
            else {

                auto loaded_models = app.client.get_loaded_models();

                if (loaded_models.empty()) {
                    ImGui::TextWrapped("No loaded models.");
                } else {
                    for (const auto& model : loaded_models) {
                        std::string label = model.model_path.empty()
                            ? model.model_ref.str()
                            : model.model_path;

                        bool selected = (app.selected_model_ref == model.model_ref);

                        if (ImGui::Selectable(label.c_str(), selected)) {
                            app.selected_model_ref = model.model_ref;
                            app.selected_model_path = model.model_path;
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();

                    if (app.selected_model_ref.isValid()) {
                        ImGui::TextWrapped(
                            "Selected model: %s",
                            app.selected_model_path.empty()
                                ? app.selected_model_ref.str().c_str()
                                : app.selected_model_path.c_str()
                        );

                        ImGui::InputText("##objectname", app.objectname, sizeof(app.objectname));

                        if (ImGui::Button("Add Object From Selected Model", ImVec2(-1, 30))) {
                            core::LoadedModelInfo info;
                            info.model_ref = app.selected_model_ref;
                            info.model_path = app.selected_model_path;

                            bool ok = app.client.add_object_from_loaded_model(info, app.objectname);
                            app.status_text = ok
                                ? "Object added from selected model"
                                : "Failed to add object from selected model";
                        }
                    }
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Import Models")) {
            if (!app.client.is_scene_ready()) {
                ImGui::TextWrapped("No active scene.");
            }
            else {
                ImGui::Text("Import Model");
                ImGui::TextWrapped("Path relative to assets folder:");
                ImGui::InputText("##import_model_path", app.import_model_path, sizeof(app.import_model_path));

                if (ImGui::Button("Import Model", ImVec2(-1, 30))) {
                    std::string path = app.import_model_path;

                    bool ok = app.client.importLocalModel(path);

                    app.status_text = ok
                        ? std::string("Imported model: ") + path
                        : std::string("Failed to import model. Missing asset: ") + path;
                }

            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

static void render(AppContext& app) {
    if (app.client.isDone()) {
        app.done = true;
    }
    process_events(app);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    draw_dockspace(app);
    draw_tools(app);
    draw_viewport(app);
    draw_right(app);
    draw_bottom(app);

    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO(); // maybe delete this

    // WSL test

    // int display_w = 0;
    // int display_h = 0;
    // SDL_GetWindowSizeInPixels(app.window, &display_w, &display_h);

    // glViewport(0, 0, display_w, display_h);
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