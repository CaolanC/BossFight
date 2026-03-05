#pragma once

#include <hv/WebSocketServer.h>
#include <thread>

namespace server
{
    class PerfectServer {
    public:
        PerfectServer(int port);
        ~PerfectServer();

        void setPort(int p);

        void start();
        void stop();

        // WS server goes here. ECS system goes
    private:
        int port;
        std::jthread ws_thread;
        hv::WebSocketService ws;
        hv::WebSocketServer ws_server;
        void async_ws_run(std::stop_token st);
        bool running = false;
    };
}