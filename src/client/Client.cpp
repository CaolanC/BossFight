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
    Client::Client(std::string name, std::string server_ip) : name(name), window(Platform::Window(name.c_str(), 1920, 1080)) {
        request_join(server_ip);
    }

    void Client::run(int w, int h) {
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