#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <platform/Window.hpp>
#include <core/MeshManager.hpp>
#include <core/ModelManager.hpp>
#include <core/Scene.hpp>

#include <NetClient.hpp>
#include <nlohmann/json.hpp>

namespace client {

    enum class InputMode {
        Editor,
        Client
    };

    class Client
    {
    public:

        Client(std::string name, std::string server_ip, bool is_editor, bool owns_window, int input_port = 0, InputMode input_mode = InputMode::Client);
        bool start(std::string server_ip2);
        Client(std::string name, std::string server_ip, bool owns_window, InputMode input_mode = InputMode::Client);
        void set_input_state(const bool* k_state);
        static void InitSDL();

        void run();
        void enter_editor(int w, int h);
        void enter_client(int w, int h);
        void handle_incoming_message(std::string& msg);
        bool connect_client(int port);
        bool is_editor;
        int input_port;
        bool start_main_loop(int w, int h);
        bool owns_window;
        std::string name;
        std::optional<Platform::Window> window;
        core::MeshManager mesh_manager = core::MeshManager();
        core::ModelManager model_manager = core::ModelManager();
        core::Scene scene;

        bool request_create_session(std::string const& ip, int& ws_port);
        //bool request_join(std::string const& ip, int input_port);
        //xg::Guid get_client_id();
        //core::Scene scene = core::Scene(mesh_manager, model_manager);
        void ensure_framebuffer(int w, int h);
        void render_to_texture(int w, int h);
        unsigned int get_render_texture() const;
        void init_embedded();

        InputMode input_mode;
        std::string server_ip;
        unsigned int framebuffer = 0;
        unsigned int color_texture = 0;
        unsigned int depth_rbo = 0;
        bool bootstrapped = false;

        int framebuffer_width = 0;
        int framebuffer_height = 0;
        void begin_input_frame();
        void process_input_event(const SDL_Event& event);
        void end_input_frame();
        void set_input_mode(InputMode mode);
        InputMode get_input_mode() const;
        void update();

        void setIsHost(bool status);
        bool getIsHost() const;

    private:
        NetClient net_client;
        xg::Guid client_id;
        //void request_join(std::string const& ip = "http://127.0.0.1:30000/join");
        bool is_host = false;

    };
}
