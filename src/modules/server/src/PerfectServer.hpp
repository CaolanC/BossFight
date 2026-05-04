#pragma once

#include <condition_variable>
#include <ServerComponents.hpp>
#include <SharedComponents.hpp>
#include "entt/entity/registry.hpp"
#include <glm/mat4x4.hpp>

#include "glm/fwd.hpp"
#include "glm/detail/type_quat.hpp"

#include <hv/WebSocketServer.h>
#include <thread>
#include <nlohmann/json.hpp>
#include <ClientInfo.hpp>

namespace server {
    class Session;
}

namespace server
{
    class PerfectServer {
    public:
        explicit PerfectServer(int port);
        ~PerfectServer();

        void setPort(int p);
        int getPort();

        void start();
        void stop();

        bool wait_until_ready(std::chrono::milliseconds timeout);

        void set_session(Session* s);

        void handle_message(const WebSocketChannelPtr& channel, const std::string& msg);
        void handle_handshake(const WebSocketChannelPtr& channel, const nlohmann::json& data);
        void handle_snapshot(const nlohmann::json& data);
        void handle_session_close(const WebSocketChannelPtr& channel);
        void handle_update(const WebSocketChannelPtr& channel, const std::string& type, const nlohmann::json& data, const std::string& raw_msg);

    private:

        entt::registry r;

        void update_loop() {

        }

        int port;
        std::jthread ws_thread;
        hv::WebSocketService ws;
        hv::WebSocketServer ws_server;
        void async_ws_run(std::stop_token st);
        std::atomic<bool> running = false;
        std::atomic<bool> ready = false;
        std::mutex ready_mutex;
        std::condition_variable ready_cv;
        Session* session;
    };
}