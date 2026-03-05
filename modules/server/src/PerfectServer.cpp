#include <PerfectServer.hpp>

namespace server
{
    PerfectServer::PerfectServer(int port) : port(port) {

    }

    PerfectServer::~PerfectServer() {
        running = false;
        stop();
    }

    void PerfectServer::setPort(int p) {
        port = p;
    }

    // WS server goes here. ECS system goes

    void PerfectServer::start() {

        ws.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
            std::cout << "WebSocket client connected\n";
        };
        ws.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg) {
            std::cout << "WS message: " << msg << "\n";
            channel->send(msg);
        };
        ws.onclose = [](const WebSocketChannelPtr& channel) {
            std::cout << "WebSocket client disconnected\n";
        };

        ws_server = hv::WebSocketServer(&ws);
        ws_server.setPort(port);
        ws_thread = std::jthread(
        [this](std::stop_token st){
            async_ws_run(st);
        }
        );

        running = true;
    }

    void PerfectServer::stop() {
        ws_server.stop();
    }

    void PerfectServer::async_ws_run(std::stop_token st) {
        std::cout << "[PerfectServer] ws_server.run() entering...\n";
        ws_server.run();
        std::cout << "[PerfectServer] ws_server.run() exited\n";
    }
};
