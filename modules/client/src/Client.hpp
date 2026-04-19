#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <platform/Window.hpp>
#include <core/MeshManager.hpp>
#include <core/ModelManager.hpp>
#include <core/Scene.hpp>

#include <NetClient.hpp>

namespace client {

    enum class InputMode {
        Editor,
        Client
    };

    class Client
    {
    public:

        Client(std::string name, std::string server_ip, bool is_editor, bool is_host, int input_port, bool owns_window, InputMode input_mode = InputMode::Client);
        //void set_input_state(const bool* k_state);
        bool start(std::string server_ip2);
        void run(int w = 100, int h = 100);
        void start_main_loop(int w, int h);
        bool owns_window;
        bool connect_client(int port);
        bool is_editor;
        bool is_host;
        int input_port;
        std::string name;
        std::optional<Platform::Window> window;
        core::MeshManager mesh_manager = core::MeshManager();
        core::ModelManager model_manager = core::ModelManager();
        core::Scene scene = core::Scene(mesh_manager, model_manager);
        void ensure_framebuffer(int w, int h);
        void render_to_texture(int w, int h);
        unsigned int get_render_texture() const;
        void init_embedded();

        InputMode input_mode;

        unsigned int framebuffer = 0;
        unsigned int color_texture = 0;
        unsigned int depth_rbo = 0;
        bool bootstrapped = false;

        int framebuffer_width = 0;
        int framebuffer_height = 0;

        bool request_create_session(std::string const& ip, int& ws_port);
        void update();
        void begin_input_frame();
        void process_input_event(const SDL_Event& event);
        void end_input_frame();

        void set_input_mode(InputMode mode);
        InputMode get_input_mode() const;
    private:
        NetClient net_client;
    };
}