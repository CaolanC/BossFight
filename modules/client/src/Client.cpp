#include <string>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <platform/Window.hpp>
#include <core/MeshManager.hpp>
#include <core/ModelManager.hpp>
#include <core/Scene.hpp>
#include <hv/HttpClient.h>
#include <Client.hpp>

namespace client {
    Client::Client(std::string name, std::string server_ip, bool is_editor, bool is_host, int input_port, bool owns_window, InputMode input_mode)
    :   name(name),
        owns_window(owns_window),
        input_mode(input_mode),
        is_editor(is_editor),
        is_host(is_host),
        input_port(input_port),
        scene(mesh_manager, model_manager, is_host)
    {
        if (owns_window) {
            window.emplace(name.c_str(), 1920, 1080);
        }
    }

    bool Client::start(std::string server_ip2) {
        if (is_host){
            int ws_port = 0;
            if (request_create_session(server_ip2, ws_port)) {
                std::cout << "Creating session on " << ws_port << "\n";
                if (connect_client(ws_port)) {
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else {
            if (connect_client(30001)) {
                std::cout << "Connected to client on port 30001\n";
                return true;
            }
            else {
                std::cout << "Failed to connect to client on port 30001\n";
                return false;
            }
        }
    }

    void Client::run(int w, int h) {
        start_main_loop(w, h);
    }

    void Client::init_embedded() {
        if (bootstrapped) return;

        scene.bootstrap(true);
        scene.set_camera_position(glm::vec3(0, 0, 1));
        bootstrapped = true;
    }

    void Client::start_main_loop(int w, int h) {
        init_embedded();
        glViewport(0, 0, w, h);

        bool quit = false;
        SDL_Event event;
        // run_init_scripts(std::ref(scene));
        scene.set_camera_position(glm::vec3(0, 0, 1));
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

            glViewport(0, 0, w, h);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            update();

            entt::registry& r = scene.getRegistry();
            systems::Render(r, w, h);

            if (window) {
                window->swap();
            }
        }
    }

    // void Client::set_input_state(const bool* k_state) {
    //     entt::registry& r = scene.getRegistry();
    //
    //     auto& kb = r.ctx().get<component::keyboard_state>();
    //     kb.k_state = k_state;
    //
    //     auto& ms  = r.ctx().get<component::mouse_state>();
    // }

    void Client::update() {
        init_embedded();

        entt::registry& r = scene.getRegistry();

        if (input_mode == InputMode::Client) {
            systems::UserControl(r);
        }

        systems::Transform(r);
        systems::Debug(r);
    }

    bool Client::request_create_session(std::string const& ip, int& ws_port) {
        hv::HttpClient cli;
        HttpRequest req;
        req.method = HTTP_GET;
        req.url = ip + "/create_session";
        req.headers["Connection"] = "keep-alive";
        req.body = "This is a sync request.";
        req.timeout = 5;
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
}