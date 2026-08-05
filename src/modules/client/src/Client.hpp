#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <core/MeshManager.hpp>
#include <core/ModelManager.hpp>
#include <core/Scene.hpp>
#include <vector>

#include <NetClient.hpp>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <Renderer.hpp>

namespace client {

    enum class InputMode {
        Editor,
        Client
    };

    class Client
    {
    public:

        Client(std::string name, bool is_editor, InputMode input_mode = InputMode::Client);
	int create_new_entity();
        bool start(std::string server_ip, int& ws_port);

        bool start_host_blank(const std::string& server_ip, int& ws_port);
        bool start_host_file(const std::string& server_ip, const std::string& file_path, int& ws_port);
        bool start_guest(const std::string& server_ip, int port);

        bool is_scene_ready() const;

        void process_network_messages();
        void handle_incoming_message(std::string& msg);
        void handle_handshake_ack();
        void handle_snapshot_message(const nlohmann::json& message);
        void handle_session_close_message();
        void handle_update_message(const std::string& type, const nlohmann::json& message);
        void poll_deferred_updates();
        bool connect_client(std::string& host, int port);

        entt::registry active_registry;
        bool is_editor;
        std::string name;
        core::MeshManager mesh_manager = core::MeshManager();
        core::ModelManager model_manager = core::ModelManager();
        core::ShaderProgramManager material_manager = core::ShaderProgramManager();
        client::Renderer renderer = client::Renderer();
        void scene_registry_migration_temorary_bootstrap();
        entt::entity spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function);

        core::Scene scene;

        bool request_create_session(std::string const& ip, int& ws_port);
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
        void begin_input_frame();
        void process_input_event(const SDL_Event& event);
        void end_input_frame();
        void set_input_mode(InputMode mode);
        InputMode get_input_mode() const;
        void update();

        std::vector<core::SerializedObject> get_scene_objects() const;
        bool get_scene_object(const std::string& object_id, core::SerializedObject& out) const;
        bool apply_gui_edit(core::SerializedObject& obj);
        bool apply_gui_delete(core::SerializedObject& obj);

        void setIsHost(bool status);
        bool getIsHost() const;

        std::vector<core::LoadedModelInfo> get_loaded_models() const;
        bool add_object_from_loaded_model(const core::LoadedModelInfo& model_info, std::string name);

        bool importLocalModel(const std::string& file_path);
        bool checkAsset(const std::string& file_path);

        bool save_to_file(std::string& file_path);
        bool save_and_quit(const std::string& file_path);
        void disconnect_and_quit();

        bool isDone();

    private:
        NetClient net_client;
        xg::Guid client_id;
        bool is_host = false;
        bool scene_ready = false;
        bool timeToShutdown = false;
        std::unordered_map<std::string, core::SerializedObject> deferred_updates;
    };
}
