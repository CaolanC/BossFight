#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <platform/Window.hpp>
#include <core/MeshManager.hpp>
#include <core/ModelManager.hpp>
#include <core/Scene.hpp>

#include <NetClient.hpp>

namespace client {
    class Client
    {
    public:

        Client(std::string name, std::string server_ip, bool is_editor);
        void run(int w = 100, int h = 100);
        void enter_editor(int w, int h);
        void enter_client(int w, int h);
        bool connect_client(int port);
        bool is_editor;
        std::string name;
        Platform::Window window;
        core::MeshManager mesh_manager = core::MeshManager();
        core::ModelManager model_manager = core::ModelManager();
        core::Scene scene = core::Scene(mesh_manager, model_manager);

        void request_join(std::string const& ip = "http://127.0.0.1:30000/join");

    private:
        NetClient net_client;
    };
}
