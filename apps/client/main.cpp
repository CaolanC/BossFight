#include <cstddef>

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <core/cameras/CameraBase.h>
#include <platform/Window.hpp>
#include <core/Mesh.hpp>
#include <core/Scene.hpp>
#include <core/defines.hpp>
#include <vector>
#include <stdio.h>
#include <map>
#include <core/MeshManager.hpp>
#include <core/scenes/Renderer.hpp>
#include <core/ShaderProgramManager.hpp>
#include <core/sh_src.hpp>
#include <entt/entt.hpp>
#include <string>
#include <hv/requests.h>
#include <hv/HttpClient.h>
#include <core/EntityManager.hpp>
#include <spawn/Spawn.hpp>

#include "systems/Render.hpp"
#include <core/ModelManager.hpp>

void request_join() {
    hv::HttpClient cli;
    HttpRequest req;
    req.method = HTTP_GET;
    req.url = "http://127.0.0.1:30000/join";
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

class Client
{
public:

    Client() {
    }

    void run() {
        int w = INIT_SCREEN_WIDTH, h = INIT_SCREEN_HEIGHT;
        glViewport(0, 0, w, h);

        bool quit = false;
        SDL_Event event;
        // run_init_scripts(std::ref(scene));
        scene.set_camera_position(glm::vec3(0, 0, 1));
        while (!quit) {
            glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            const bool* k_state = SDL_GetKeyboardState(NULL); // TODO: Singleton component for game input
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
private:
    Platform::Window window = Platform::Window(SLJA_WINDOW_TITLE, 1920, 1080);
    core::MeshManager mesh_manager = core::MeshManager();
    core::ModelManager model_manager = core::ModelManager();
    core::Scene scene = core::Scene(mesh_manager, model_manager);
};

int main() {
    request_join();
    Client client;
    client.run();
    return 0;
};
 