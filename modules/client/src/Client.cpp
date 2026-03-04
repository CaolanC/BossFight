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
    Client::Client(std::string name, std::string server_ip, bool is_editor) : name(name), window(Platform::Window(name.c_str(), 1920, 1080)), is_editor(is_editor) {
        request_join(server_ip);
    }

    void Client::run(int w, int h) {

        if (is_editor) {
            enter_editor(w, h);
        } else {
            enter_client(w, h);
        }
    }

void Client::enter_editor(int w, int h) {
    glViewport(0, 0, w, h);

    bool quit = false;
    SDL_Event event;

    scene.set_camera_position(glm::vec3(0, 0, 1));

    // You probably already did ImGui::CreateContext() + Impl init somewhere before.

    while (!quit) {
        // 1) Poll events
        while (SDL_PollEvent(&event)) {
            // Feed events to ImGui first

            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // 2) Start a new ImGui frame

        // 3) Get FBO + (optionally) its color texture + size from scene
        GLuint sceneFbo       = scene.get_fbo();

        // 4) Render world into the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.update();   // inside here you call your systems::NewRender(reg)

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

        // 5) Build ImGui UI, including the scene viewport
        //    (for now just make it fill the window)
        // Optional: if size changed, tell scene to resize its FBO
        // scene.resize_render_target((int)size.x, (int)size.y);

        // TODO later: add Hierarchy, Inspector, etc.

        // 6) Render ImGui to the default framebuffer
        glViewport(0, 0, w, h);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 7) Present
        window.swap();
    }
}


    void Client::enter_client(int w, int h) {
        glViewport(0, 0, w, h);

        bool quit = false;
        SDL_Event event;
        // run_init_scripts(std::ref(scene));
        scene.set_camera_position(glm::vec3(0, 0, 1));
        while (!quit) {
            glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            scene.update();
            while (SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_EVENT_QUIT:
                        quit = true;
                        break;
                }
            }
            window.swap();
        };
    }

    void Client::request_join(std::string const& ip) {
        hv::HttpClient cli;
        HttpRequest req;
        req.method = HTTP_GET;
        req.url = ip;
        req.headers["Connection"] = "keep-alive";
        req.body = "This is a sync request.";
        req.timeout = 10;
        HttpResponse resp;
        int ret = cli.send(&req, &resp);
        if (ret != 0) {
            printf("request failed!\n");
        } else {
            printf("%d %s\r\n", resp.status_code, resp.status_message());
            printf("%s %s\n", resp.body.c_str(), resp.headers["Connection"].c_str());
        }
    };
}