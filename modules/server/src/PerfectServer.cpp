#include <PerfectServer.hpp>

#include "nlohmann/json.hpp"
#include <crossguid/guid.hpp>

#include "Session.hpp"

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

        if (running) {
            std::cout << "[PerfectServer] start() called but already running\n";
            return;
        }

        running = true;
        ready = false;

        ws.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
            std::cout << "WebSocket client connected\n";
        };
        ws.onmessage = [this](const WebSocketChannelPtr& channel, const std::string& msg) {
            nlohmann::json data = nlohmann::json::parse(msg);
            std::string type = data.at("type").get<std::string>();
            std::string role = data.at("payload").at("role").get<std::string>();
            std::string ci = data.at("payload").at("client_id").get<std::string>();
            xg::Guid client_id(ci);
            std::cout << client_id << "\n";

            if (type == "handshake"){
                ClientInfo client_info = ClientInfo();
                client_info.client_id = client_id;
                client_info.role = role;
                session->addClient(channel, client_info);
                if (role == "host") {
                    std::cout << "Host handshake, request snapshot\n";
                }
                else {
                    std::cout << "Guest handshake, send snapshot\n";
                }
            }
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
        });

    }

    void PerfectServer::stop() {
        ws_server.stop();
    }

    void PerfectServer::async_ws_run(std::stop_token st) {
        {
            std::lock_guard<std::mutex> lock(ready_mutex);
            ready = true;
        }
        ready_cv.notify_all();
        std::cout << "[PerfectServer] ws_server.run() entering...\n";
        ws_server.run();
        running = false;
        ready = false;
        std::cout << "[PerfectServer] ws_server.run() exited\n";
    }

    bool PerfectServer::wait_until_ready(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(ready_mutex);
        return ready_cv.wait_for(lock, timeout, [this]() { return ready.load(); });
    }

    void PerfectServer::set_session(Session* s) {
        session = s;
    }
};
