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
#include <entt/entt.hpp>
#include <spawn/Spawn.hpp>
#include <crossguid/guid.hpp>

#include "hv/json.hpp"

namespace client {

    Client::Client(std::string name, bool is_editor, InputMode input_mode)
        :   name(name),
            input_mode(input_mode),
            is_editor(is_editor)
            // scene(mesh_manager, model_manager)
    {
        client_id = xg::newGuid();
        mesh_manager = mesh_manager;
        model_manager = model_manager;
        scene_registry_migration_temorary_bootstrap(); 
    }

    void Client::scene_registry_migration_temorary_bootstrap() {
        active_registry.ctx().emplace<component::current_camera>(spawn(spawn::freecam));
    }

    xg::Guid Client::add_test_model() {
        return model_manager.add_model(model_loader.load_model("models/sink/scene.gltf"));
    }

    entt::entity Client::spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function) {
         return spawn_function(std::ref(active_registry));
    }

    int Client::create_new_entity() {
        return 0;
    }

    // Gets the host from the URL (removes the http, any : and any /)

    static std::string extract_host_from_http_url(std::string& url) {
        std::string s = url;

        const std::string http = "http://";
        const std::string https = "https://";

        if (s.rfind(http, 0) == 0) {
            s = s.substr(http.size());
        } else if (s.rfind(https, 0) == 0) {
            s = s.substr(https.size());
        }

        auto slash_pos = s.find('/');
        if (slash_pos != std::string::npos) {
            s = s.substr(0, slash_pos);
        }

        auto colon_pos = s.find(':');
        if (colon_pos != std::string::npos) {
            s = s.substr(0, colon_pos);
        }

        return s;
    }

    bool Client::start(std::string server_ip, int& ws_port) {
        // std::string ws_host = extract_host_from_http_url(server_ip);
        // if (is_host){
        //     // int ws_port = 0;
        //     if (request_create_session(server_ip, ws_port)) {
        //         std::cout << "Created session on " << ws_port << "\n";
        //         if (connect_client(ws_host, ws_port)) {
        //             std::cout << "Connected to client\n";
        //             return true;
        //         }
        //         else {
        //             std::cout << "Failed to connect to sws on port " << ws_port << ".\n";
        //             return false;
        //         }
        //     }
        //     else {
        //         return false;
        //     }
        // }
        // else {
        //     if (connect_client(ws_host, ws_port)) {
        //         std::cout << "Connected to client on port " << ws_port << "\n";
        //         return true;
        //     }
        //     else {
        //         std::cout << "Failed to connect to client on port " << ws_port << "\n";
        //         return false;
        //     }
        // }
    }

    // 3 functions for starting scene based on user input (host blank, host from file, join as guest)
    // Each set respective host status, call the start function above, and initialize after
    // Then send a handshake message to server

    // bool Client::start_host_blank(const std::string& server_ip, int& ws_port) {
    //     setIsHost(true);

    //     //if (!(start(server_ip, ws_port))){
    //     //    scene_ready = false;
    //     //    return false;
    //     //}

    //     scene.bootstrap();
    //     scene.set_camera_position(glm::vec3(0, 3, 3));
    //     scene_ready = true;

    //     //net_client.send(shared::JSONHelper::make_handshake(client_id, true));

    //     return true;
    // }

    // bool Client::start_host_file(const std::string& server_ip, const std::string& file_path, int& ws_port) {
    //     setIsHost(true);

    //     if (!(utils::assets::filepath_exists(file_path))) {
    //         scene_ready = false;
    //         return false;
    //     }

    //     if (!(start(server_ip, ws_port))){
    //         scene_ready = false;
    //         return false;
    //     }

    //     bool ok = scene.bootstrap_from_file(file_path);
    //     if (!ok) {
    //         scene_ready = false;
    //         net_client.send(shared::JSONHelper::make_session_closed_message());
    //         return false;
    //     }

    //     scene.set_camera_position(glm::vec3(0, 3, 3));
    //     scene_ready = true;

    //     net_client.send(shared::JSONHelper::make_handshake(client_id, true));

    //     return true;
    // }

    // bool Client::start_guest(const std::string &server_ip, int port) {
    //     net_client.disconnect();
    //     setIsHost(false);

    //     if(!(start(server_ip, port))) {
    //         scene_ready = false;
    //         return false;
    //     }

    //     scene.bootstrap();
    //     scene.set_camera_position(glm::vec3(0, 3, 3));
    //     scene_ready = false;

    //     net_client.send(shared::JSONHelper::make_handshake(client_id, false));

    //     return true;
    // }

    void Client::init_embedded() {
        if (bootstrapped) return;

        scene_ready = false;
        bootstrapped = true;
    }

    // Updates scene registry

    void Client::update() {

        // entt::registry& r = scene.getRegistry();

        if (input_mode == InputMode::Client) {
            systems::UserControl(active_registry);
        }

        systems::Transform(active_registry);
        systems::Debug(active_registry);
    }

    // Processes all incoming WebSockets messages

    // void Client::process_network_messages() {
    //     std::string msg;

    //     while (net_client.pollMessage(msg)) {
    //         handle_incoming_message(msg);
    //     }
    // }

    // Handles said message above, different behaviour for each message type

    // void Client::handle_incoming_message(std::string& msg) {
    //     nlohmann::json message = nlohmann::json::parse(msg);
    //     std::string type = message.at("type").get<std::string>();

    //     if (type == "handshake_ack") {
    //         handle_handshake_ack();
    //     }
    //     else if (type == "snapshot") {
    //         handle_snapshot_message(message);
    //     }
    //     else if (type == "session_close") {
    //         handle_session_close_message();
    //     }
    //     else {
    //         handle_update_message(type, message);
    //     }
    // }

    // Handshake acknowledgment handling. Host client sends snapshot to server
    // void Client::handle_handshake_ack() {
    //     if (is_host) {
    //         std::cout << "Handshake acknowledged. Sending snapshot.\n";
    //         net_client.send(shared::JSONHelper::make_snapshot_message(scene.get_initial_snapshot()));
    //     }
    // }

    // Snapshot message handling. Guest takes in snapshot and sets it to scene - if doesn't have
    // local assets, send to deferred snapshot
    // void Client::handle_snapshot_message(const nlohmann::json& message) {
    //     if (!is_host) {
    //         std::cout << "Snapshot received. Preparing to initialize...\n";
    //         nlohmann::json message_payload = message.at("payload");
    //         core::SceneSnapshot snapshot = shared::JSONHelper::deserialize_snapshot_string(message_payload.dump());

    //         core::SceneSnapshot snapshot_no_defers;

    //         for (const auto& [key, obj] : snapshot.getmap()) {
    //             if (checkAsset(obj.model_path)) {
    //                 snapshot_no_defers.insert(key, obj);
    //             }
    //             else {
    //                 // If the asset does not exist locally, send it to the deferred updates list
    //                 deferred_updates[obj.objectID] = obj;
    //                 std::cout << "Deferred snapshot object. Missing asset: " << obj.model_path << "\n";
    //             }
    //         }
    //         scene.guest_init(snapshot_no_defers);
    //         scene_ready = true;
    //     }
    // }

    // Session close handling. Disconnect and quit
    // void Client::handle_session_close_message() {
    //     std::cout << "Session closed by host\n";
    //     disconnect_and_quit();
    // }

    // // Update message handling. Different for each type.
    // // ADD UPDATE: Add object to scene. Send to deferred updates if asset doesn't exist locally.
    // // EDIT UPDATE: Edit object in local EnTT registry, if asset doesn't exist overwrite deferred update.
    // // DELETE UPDATE: Delete object from local EnTT registry. Delete from deferred update if local assets don't exist.
    // void Client::handle_update_message(const std::string& type, const nlohmann::json& message) {
    //     nlohmann::json message_payload = message.at("payload");
    //     core::SerializedObject obj = shared::JSONHelper::deserialize_object_string(message_payload.dump());
    //     if (type == "update_add") {
    //         if (!(checkAsset(obj.model_path))) {
    //             deferred_updates[obj.objectID] = obj;
    //             std::cout << "Deferred add. Missing asset: " << obj.model_path << "\n";
    //             return;
    //         }
    //         bool ok = scene.add_obj(obj);
    //     }
    //     else if (type == "update_edit") {
    //         if (deferred_updates.contains(obj.objectID)) {
    //             deferred_updates[obj.objectID] = obj;
    //             std::cout << "Updated deferred object: " << obj.objectID << "\n";
    //             return;
    //         }
    //         bool ok = scene.edit_obj(obj);
    //     }
    //     else if (type == "update_delete") {
    //         if (deferred_updates.contains(obj.objectID)) {
    //             deferred_updates.erase(obj.objectID);
    //         }
    //         if (scene.check_registry(obj.objectID)) {
    //             bool ok = scene.delete_obj(obj);
    //         }
    //     }
    // }

    // Polls the deferred update map, increments through a pointer (it).
    // void Client::poll_deferred_updates() {
    //     for (auto it = deferred_updates.begin(); it != deferred_updates.end(); ) {
    //         core::SerializedObject obj = it->second;

    //         if (!(checkAsset(obj.model_path))) {
    //             it++;
    //             continue;
    //         }

    //         bool ok = scene.add_obj(obj);

    //         if (ok) {
    //             it = deferred_updates.erase(it);
    //         }
    //         else {
    //             ++it;
    //         }

    //     }
    // }

    // Basic HTTP request for session creation
    // bool Client::request_create_session(std::string const& ip, int& ws_port) {
    //     hv::HttpClient cli;
    //     HttpRequest req;
    //     req.method = HTTP_GET;
    //     req.url = ip + "/create_session";
    //     std::cout << "[CLIENT] Making a create session request to: " << req.url << "\n";
    //     req.headers["Connection"] = "keep-alive";
    //     req.body = "This is a sync request.";
    //     req.timeout = 10;
    //     HttpResponse resp;
    //     int ret = cli.send(&req, &resp);
    //     if (ret != 0 || resp.status_code != 200) {
    //         printf("request failed!\n");
    //         return false;
    //     } else {
    //         printf("%d %s\r\n", resp.status_code, resp.status_message());
    //         printf("%s %s\n", resp.body.c_str(), resp.headers["Connection"].c_str());

    //         std::istringstream iss(resp.body);
    //         std::string status, sid;

    //         if (!(iss >> status >> sid >> ws_port)){
    //             return false;
    //         }

    //         return status == "ok";
    //     }
    // };

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

        // entt::registry& r = scene.getRegistry();
        //systems::Render(r, w, h);
        renderer.render(active_registry, w, h, model_manager, material_manager);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int Client::get_render_texture() const {
        return color_texture;
    }

    void Client::begin_input_frame() {
        entt::registry& r = active_registry;
        auto& ms = active_registry.ctx().get<component::mouse_state>();

        ms.dx = 0.0f;
        ms.dy = 0.0f;
    }

    // Processes user input
    void Client::process_input_event(const SDL_Event& event) {
        entt::registry& r = active_registry;
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

            case SDL_EVENT_MOUSE_MOTION: {
                float rel_x = event.motion.xrel;
                float rel_y = event.motion.yrel;

                // Fallback for Linux/VirtualBox
                if (rel_x == 0.0f && rel_y == 0.0f) {
                    rel_x = event.motion.x - ms.x;
                    rel_y = event.motion.y - ms.y;
                }

                ms.x = event.motion.x;
                ms.y = event.motion.y;
                ms.dx += rel_x;
                ms.dy += rel_y;
                break;
            }

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

    // Connects to WebSocket port
    bool Client::connect_client(std::string& host, int port) {
        for (int i = 0; i < 20; ++i) {
            if (net_client.is_connected()) {
                return true;
            }

            if (!net_client.is_connecting()) {
                net_client.connect(host, port);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        return net_client.is_connected();
    }

    // Set host status.
    void Client::setIsHost(bool status) {
        is_host = status;
    }

    // Get host status.
    bool Client::getIsHost() const {
        return is_host;
    }

    // Is the scene ready?
    bool Client::is_scene_ready() const {
        return scene_ready;
    }

    // Gets object list from scene.
    // std::vector<core::SerializedObject> Client::get_scene_objects() const {
    //     return scene.get_object_info();
    // }

    // Gets a specific object from the scene's registry.
    // bool Client::get_scene_object(const std::string& object_id, core::SerializedObject& out) const {
    //     return scene.registry_lookup_to_obj(object_id, out);
    // }

    // Applies local edit if called from the GUI. Sends relevant message.
    // bool Client::apply_gui_edit(core::SerializedObject &obj) {
    //     bool ok = scene.edit_obj(obj);

    //     if (ok) {
    //         net_client.send(shared::JSONHelper::make_update_message("edit", obj));
    //     }

    //     return ok;
    // }

    // Applies local deletion if called from the GUI. Sends relevant message.
    // bool Client::apply_gui_delete(core::SerializedObject &obj) {
    //     bool ok = scene.delete_obj(obj);

    //     if (ok) {
    //         net_client.send(shared::JSONHelper::make_update_message("delete", obj));
    //     }

    //     return ok;
    // }

    // Get the loaded model list from the scene.
    // std::vector<core::LoadedModelInfo> Client::get_loaded_models() const {
    //     return scene.get_loaded_models();
    // }

    // Adds an object from the loaded model list. Auto sets name to "Object" if none is supplied.
    // bool Client::add_object_from_loaded_model(const core::LoadedModelInfo& model_info, std::string name) {
    //     if (name.empty()) {
    //         name = "Object";
    //     }
    //     core::SerializedObject obj;
    //     obj.objectID = xg::newGuid().str();
    //     obj.model_ref = model_info.model_ref;
    //     obj.name = name;
    //     obj.model_path = model_info.model_path;
    //     obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
    //     obj.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    //     obj.scale = 1.0f;

    //     bool ok = scene.add_obj(obj);

    //     if (ok) {
    //         net_client.send(shared::JSONHelper::make_update_message("add", obj));
    //     }

    //     return ok;
    // }

    // Imports model. If asset doesn't exist, don't go through.
    // bool Client::importLocalModel(const std::string& file_path) {
    //     if (!(checkAsset(file_path))) {
    //         return false;
    //     }
    //     bool ok = scene.load_model_from_gui(file_path);

    //     if (ok) {
    //         poll_deferred_updates();
    //     }

    //     return ok;
    // }

    // Asset checking function. Calls model_file_exists in util::assets. Placed here so GUI can access.
    bool Client::checkAsset(const std::string& file_path) {
        return utils::assets::model_file_exists(file_path);
    }

    // Save (write) to a file.
    // bool Client::save_to_file(std::string& file_path) {
    //     core::SceneSnapshot save_snapshot = scene.build_snapshot();
    //     return core::SceneSerializer::save(save_snapshot, utils::assets::get_filepath(file_path));
    // }

    // Save and quit the application. Send session_close message to WebSockets.
    // bool Client::save_and_quit(const std::string& file_path) {
    //     if (!is_host) {
    //         return false;
    //     }

    //     std::string jsonfile;

    //     if (file_path.empty()) {
    //         jsonfile = "blank.json";
    //     }
    //     else {
    //         jsonfile = file_path + ".json";
    //     }

    //     bool ok = save_to_file(jsonfile);

    //     if (!ok) {
    //         return false;
    //     }

    //     net_client.send(shared::JSONHelper::make_session_closed_message());
    //     return true;
    // }

    // Disconnect and quit the application (or set the bool that decides it).
    void Client::disconnect_and_quit() {
        net_client.disconnect();

        timeToShutdown = true;
    }

    // Return boolean that says the client is ready to shut down.
    bool Client::isDone() {
        return timeToShutdown;
    }

}
