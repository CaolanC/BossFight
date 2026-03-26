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
    Client::Client(std::string name, std::string server_ip, bool is_editor, bool is_host)
    :   name(name),
        window(Platform::Window(name.c_str(), 1920, 1080)),
        is_editor(is_editor),
        is_host(is_host),
        scene(mesh_manager, model_manager, is_host)
    {
        request_create_session(server_ip);

        connect_client(30001);
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

    while (!quit) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        GLuint sceneFbo       = scene.get_fbo();

        glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.update();

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

        glViewport(0, 0, w, h);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


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

    void Client::request_create_session(std::string const& ip) {
        hv::HttpClient cli;
        HttpRequest req;
        req.method = HTTP_GET;
        req.url = ip + "/create_session";
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
}