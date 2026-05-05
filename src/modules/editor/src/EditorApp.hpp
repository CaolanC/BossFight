#pragma once

#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include <string>
#include <crossguid/guid.hpp>
#include <SDL3/SDL.h>
#include <Client.hpp>
#include <SerializedObject.hpp>

namespace gui {
    struct AppContext {
        SDL_Window* window = nullptr;
        SDL_GLContext gl_context = nullptr;

        client::Client client{
            "Perfect Client.",
            true,
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
        std::string session_info = "No active session.";
        int session_port = 0;

        std::string selected_object_id;
        core::SerializedObject selected_object{};

        xg::Guid selected_model_ref{};
        std::string selected_model_path;

        char import_model_path[260] = "";
        char objectname[128] = "";

    };
}