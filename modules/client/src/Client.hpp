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

        Client(std::string name, std::string server_ip, bool is_editor, bool is_host, int input_port = 0);
        bool start(std::string server_ip2);
        void run(int w = 100, int h = 100);
        void enter_editor(int w, int h);
        void enter_client(int w, int h);
        bool connect_client(int port);
        bool is_editor;
        bool is_host;
        int input_port;
        std::string name;
        Platform::Window window;
        core::MeshManager mesh_manager = core::MeshManager();
        core::ModelManager model_manager = core::ModelManager();
        core::Scene scene;

        bool request_create_session(std::string const& ip, int& ws_port);
        bool request_join(std::string const& ip, int input_port);
        xg::Guid get_client_id();

    private:
        NetClient net_client;
        xg::Guid client_id;
    };
}
