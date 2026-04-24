#include <string>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <platform/Window.hpp>
#include <core/MeshManager.hpp>
#include <core/ModelManager.hpp>
#include <core/Scene.hpp>
#include <hv/HttpClient.h>
#include <Client.hpp>
#include <JSONHelper.hpp>

#include "hv/json.hpp"

namespace client {

    Client::Client(std::string name, std::string server_ip, bool is_editor, bool owns_window, int input_port, InputMode input_mode)
        :   name(name),
            owns_window(owns_window),
            input_mode(input_mode),
            is_editor(is_editor),
            input_port(input_port),
            scene(mesh_manager, model_manager),
            server_ip(server_ip)
    {
        client_id = xg::newGuid();
        if (owns_window) {
            window.emplace(name.c_str(), 1920, 1080);
        }
        // request_join(server_ip);
    }

    bool Client::start(std::string server_ip2) {
        if (is_host){
            int ws_port = 0;
            if (request_create_session(server_ip2, ws_port)) {
                std::cout << "Created session on " << ws_port << "\n";
                if (connect_client(ws_port)) {
                    net_client.send(shared::JSONHelper::make_handshake(client_id, true));
                    return true;
                }
                else {
                    std::cout << "Failed to connect to sws on port " << ws_port << ".\n";
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else {
            if (connect_client(input_port)) {
                std::cout << "Connected to client on port 30001\n";
                net_client.send(shared::JSONHelper::make_handshake(client_id, false));
                return true;
            }
            else {
                std::cout << "Failed to connect to client on port 30001\n";
                return false;
            }
        }
    }

    bool Client::start_host_blank(const std::string& server_ip2) {
        setIsHost(true);

        scene.bootstrap();
        scene.set_camera_position(glm::vec3(0, 0, 1));
        scene_ready = true;

        return start(server_ip2);
    }

    bool Client::start_host_file(const std::string &server_ip2, const std::string &file_path) {
        setIsHost(true);

        scene.bootstrap_from_file(file_path);
        scene.set_camera_position(glm::vec3(0, 0, 1));
        scene_ready = true;

        return start(server_ip2);
    }

    bool Client::start_guest(const std::string &server_ip2, int port) {
        setIsHost(false);
        setInputPort(port);

        scene.bootstrap();
        scene.set_camera_position(glm::vec3(0, 0, 1));
        scene_ready = false;

        return start(server_ip2);
    }


    void Client::run() {
        bool started = start_main_loop(0, 0);
    }

    void Client::InitSDL() {

    }

    void Client::init_embedded() {
        if (bootstrapped) return;

        scene_ready = false;
        bootstrapped = true;
    }

    bool Client::start_main_loop(int w, int h) {
        init_embedded();
        if (start(server_ip)) {
            glViewport(0, 0, w, h);

            bool quit = false;
            SDL_Event event;

            while (!quit) {
                begin_input_frame();

                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT:
                            quit = true;
                            break;
                    }

                    process_input_event(event);
                }

                end_input_frame();

                int draw_w = w;
                int draw_h = h;

                if (window) {
                    SDL_GetWindowSizeInPixels(window->get_window(), &draw_w, &draw_h);
                }

                glViewport(0, 0, draw_w, draw_h);
                glEnable(GL_DEPTH_TEST);
                glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                std::string msg;
                while (net_client.pollMessage(msg)) {
                    handle_incoming_message(msg);
                }

                update();

                entt::registry& r = scene.getRegistry();
                systems::Render(r, draw_w, draw_h);

                if (window) {
                    window->swap();
                }

            }

            return true;
        }
    }

    // void Client::set_input_statsse(const bool* k_state) {
    //     entt::registry& r = scene.getRegistry();
    //
    //     auto& kb = r.ctx().get<component::keyboard_state>();
    //     kb.k_state = k_state;
    //
    //     auto& ms  = r.ctx().get<component::mouse_state>();
    // }

    void Client::update() {
        // init_embedded();

        entt::registry& r = scene.getRegistry();

        if (input_mode == InputMode::Client) {
            systems::UserControl(r);
        }

        systems::Transform(r);
        systems::Debug(r);
    }

    void Client::process_network_messages() {
        std::string msg;

        while (net_client.pollMessage(msg)) {
            handle_incoming_message(msg);
        }
    }

    void Client::handle_incoming_message(std::string& msg) {
        nlohmann::json message = nlohmann::json::parse(msg);
        std::string type = message.at("type").get<std::string>();

        if (type == "handshake_ack") {
            if (is_host) {
                std::cout << "Handshake acknowledged. Sending snapshot.\n";
                net_client.send(shared::JSONHelper::make_snapshot_message(scene.get_initial_snapshot()));
            }
        }
        else if (type == "snapshot") {
            if (!is_host) {
                std::cout << "Snapshot received. Preparing to initialize...\n";
                nlohmann::json message_payload = message.at("payload");
                // std::cout << message_payload.dump() << "\n";
                core::SceneSnapshot snapshot = shared::JSONHelper::deserialize_snapshot_string(message_payload.dump());

                core::SceneSnapshot snapshot_no_defers;

                for (const auto& [key, obj] : snapshot.getmap()) {
                    if (checkAsset(obj.model_path)) {
                        snapshot_no_defers.insert(key, obj);
                    }
                    else {
                        deferred_updates[obj.objectID] = obj;
                        std::cout << "Deferred snapshot object. Missing asset: " << obj.model_path << "\n";
                    }
                }
                scene.guest_init(snapshot_no_defers);
                scene_ready = true;
            }
        }
        else {
            nlohmann::json message_payload = message.at("payload");
            core::SerializedObject obj = shared::JSONHelper::deserialize_object_string(message_payload.dump());
            if (type == "update_add") {
                if (!(checkAsset(obj.model_path))) {
                    deferred_updates[obj.objectID] = obj;
                    std::cout << "Deferred add. Missing asset: " << obj.model_path << "\n";
                    return;
                }
                bool ok = scene.add_obj(obj);
            }
            else if (type == "update_edit") {
                if (deferred_updates.contains(obj.objectID)) {
                    deferred_updates[obj.objectID] = obj;
                    std::cout << "Updated deferred object: " << obj.objectID << "\n";
                    return;
                }
                bool ok = scene.edit_obj(obj);
            }
            else if (type == "update_delete") {
                if (deferred_updates.contains(obj.objectID)) {
                    deferred_updates.erase(obj.objectID);
                }
                if (scene.check_registry(obj.objectID)) {
                    bool ok = scene.delete_obj(obj);
                }
            }
        }
    }

    void Client::poll_deferred_updates() {
        for (auto it = deferred_updates.begin(); it != deferred_updates.end(); ) {
            core::SerializedObject obj = it->second;

            if (!(checkAsset(obj.model_path))) {
                it++;
                continue;
            }

            bool ok = scene.add_obj(obj);

            if (ok) {
                it = deferred_updates.erase(it);
            }
            else {
                ++it;
            }

        }
    }


    bool Client::request_create_session(std::string const& ip, int& ws_port) {
        hv::HttpClient cli;
        HttpRequest req;
        req.method = HTTP_GET;
        req.url = ip + "/create_session";
        req.headers["Connection"] = "keep-alive";
        req.body = "This is a sync request.";
        req.timeout = 10;
        HttpResponse resp;
        int ret = cli.send(&req, &resp);
        if (ret != 0 || resp.status_code != 200) {
            printf("request failed!\n");
            return false;
        } else {
            printf("%d %s\r\n", resp.status_code, resp.status_message());
            printf("%s %s\n", resp.body.c_str(), resp.headers["Connection"].c_str());

            std::istringstream iss(resp.body);
            std::string status, sid;

            if (!(iss >> status >> sid >> ws_port)){
                return false;
            }

            return status == "ok";
        }
    };

    void Client::ensure_framebuffer(int w, int h) {
        if (framebuffer != 0 && framebuffer_width == w && framebuffer_height == h) {
            return;
        }

        if (framebuffer != 0) {
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &color_texture);
            glDeleteRenderbuffers(1, &depth_rbo);
        }

        framebuffer_width = w;
        framebuffer_height = h;

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &color_texture);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

        glGenRenderbuffers(1, &depth_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::printf("framebuffer incomplete\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Client::render_to_texture(int w, int h) {
        init_embedded();
        ensure_framebuffer(w, h);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, w, h);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        entt::registry& r = scene.getRegistry();
        systems::Render(r, w, h);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int Client::get_render_texture() const {
        return color_texture;
    }

    void Client::begin_input_frame() {
        entt::registry& r = scene.getRegistry();
        auto& ms = r.ctx().get<component::mouse_state>();

        ms.dx = 0.0f;
        ms.dy = 0.0f;
    }

    void Client::process_input_event(const SDL_Event& event) {
        entt::registry& r = scene.getRegistry();
        auto& kb = r.ctx().get<component::keyboard_state>();
        auto& ms = r.ctx().get<component::mouse_state>();

        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.scancode < SDL_SCANCODE_COUNT) {
                    kb.down[event.key.scancode] = true;
                }
                break;

            case SDL_EVENT_KEY_UP:
                if (event.key.scancode < SDL_SCANCODE_COUNT) {
                    kb.down[event.key.scancode] = false;
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                ms.x = event.motion.x;
                ms.y = event.motion.y;
                ms.dx += event.motion.xrel;
                ms.dy += event.motion.yrel;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button < ms.down.size()) {
                    ms.down[event.button.button] = true;
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button < ms.down.size()) {
                    ms.down[event.button.button] = false;
                }
                break;

            default:
                break;
        }
    }

    void Client::end_input_frame() {
    }

    void Client::set_input_mode(InputMode mode) {
        input_mode = mode;
    }

    InputMode Client::get_input_mode() const {
        return input_mode;
    }

    bool Client::connect_client(int port) {
        for (int i = 0; i < 20; ++i) {
            if (net_client.is_connected()) {
                return true;
            }

            if (!net_client.is_connecting()) {
                net_client.connect(port);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        return net_client.is_connected();
    }

    void Client::setIsHost(bool status) {
        is_host = status;
    }

    bool Client::getIsHost() const {
        return is_host;
    }

    bool Client::is_scene_ready() const {
        return scene_ready;
    }

    void Client::setInputPort(int port) {
        input_port = port;
    }

    std::vector<core::SerializedObject> Client::get_scene_objects() const {
        return scene.get_object_info();
    }

    bool Client::get_scene_object(const std::string& object_id, core::SerializedObject& out) const {
        return scene.registry_lookup_to_obj(object_id, out);
    }

    bool Client::apply_gui_edit(core::SerializedObject &obj) {
        bool ok = scene.edit_obj(obj);

        if (ok) {
            net_client.send(shared::JSONHelper::make_update_message("edit", obj));
        }

        return ok;
    }

    bool Client::apply_gui_delete(core::SerializedObject &obj) {
        bool ok = scene.delete_obj(obj);

        if (ok) {
            net_client.send(shared::JSONHelper::make_update_message("delete", obj));
        }

        return ok;
    }


    std::vector<core::LoadedModelInfo> Client::get_loaded_models() const {
        return scene.get_loaded_models();
    }

    bool Client::add_object_from_loaded_model(const core::LoadedModelInfo& model_info) {
        core::SerializedObject obj;
        obj.objectID = xg::newGuid().str();
        obj.model_ref = model_info.model_ref;
        obj.model_path = model_info.model_path;
        obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
        obj.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        obj.scale = 1.0f;

        bool ok = scene.add_obj(obj);

        if (ok) {
            net_client.send(shared::JSONHelper::make_update_message("add", obj));
        }

        return ok;
    }

    bool Client::importLocalModel(const std::string& file_path) {
        if (!(checkAsset(file_path))) {
            return false;
        }
        bool ok = scene.load_model_from_gui(file_path);

        if (ok) {
            poll_deferred_updates();
        }

        return ok;
    }

    bool Client::checkAsset(const std::string& file_path) {
        return utils::assets::asset_exists(file_path);
    }


}
