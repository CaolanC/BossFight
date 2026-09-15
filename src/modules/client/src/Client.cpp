#include <glad/glad.h>
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
#include <core/sh_src.hpp>
#include "hv/json.hpp"
#include <rendering/ShaderSource.hpp>

namespace client {

    Client::Client(std::string name, bool is_editor, InputMode input_mode)
        :   name(name),
            input_mode(input_mode),
            is_editor(is_editor)
            // scene(mesh_manager, model_manager)
    {
        client_id = xg::newGuid();
        // mesh_manager = mesh_manager;
        // model_manager = model_manager; 
        
    };

    void Client::scene_registry_migration_temorary_bootstrap() {
	if (glCreateShader == nullptr) {
        SDL_Log("ERROR: glCreateShader is still NULL inside bootstrap!");
        return;
    	}

		resource_manager.init();
		renderer.init_ubos();

        active_registry.ctx().emplace<component::keyboard_state>();
        active_registry.ctx().emplace<component::mouse_state>();
        active_registry.ctx().emplace<component::current_camera>(spawn(spawn::freecam));
    };

    entt::entity Client::spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function) {
    	return spawn_function(std::ref(active_registry));
    };

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
    }

    void Client::init_embedded() {
        if (bootstrapped) return;

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            SDL_Log("Failed to initialize GLAD in init_embedded!");
            return;
    	}


	
        scene_registry_migration_temorary_bootstrap();

        scene_ready = true;
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
	renderer.new_render(active_registry, w, h);
        //renderer.render(active_registry, w, h, model_manager, material_manager);
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

    // Asset checking function. Calls model_file_exists in util::assets. Placed here so GUI can access.
    bool Client::checkAsset(const std::string& file_path) {
        return utils::assets::model_file_exists(file_path);
    }

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
