#include <PerfectServer.hpp>

#include <crossguid/guid.hpp>

#include "Session.hpp"
#include <JSONHelper.hpp>
#include <SceneSnapshot.hpp>
#include <vector>

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

    int PerfectServer::getPort() {
        return port;
    }

    // WS server code adapted from libhv documentation (https://github.com/ithewei/libhv
    // at WebSockets server)

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
            handle_message(channel, msg);
        };

        ws.onclose = [this](const WebSocketChannelPtr& channel) {
            std::cout << "WebSocket client disconnected\n";
            session->removeClient(channel);
        };

        ws_server = hv::WebSocketServer(&ws);
        ws_server.setPort(port);
        ws_thread = std::jthread(
        [this](std::stop_token st){
            async_ws_run(st);
        });

    }

    void PerfectServer::handle_message(const WebSocketChannelPtr& channel, const std::string& msg) {
        nlohmann::json data = nlohmann::json::parse(msg);
        std::string type = data.at("type").get<std::string>();

        if (type == "handshake"){
            handle_handshake(channel, data);
        }
        else if (type == "snapshot") {
            handle_snapshot(data);
        }
        else if (type == "session_close") {
            handle_session_close(channel);
        }
        else {
            handle_update(channel, type, data, msg);
        }
    }

    void PerfectServer::handle_handshake(const WebSocketChannelPtr& channel, const nlohmann::json& data) {
        std::string role = data.at("payload").at("role").get<std::string>();
        std::string ci = data.at("payload").at("client_id").get<std::string>();
        xg::Guid client_id(ci);
        std::cout << client_id << "\n";

        ClientInfo client_info = ClientInfo();
        client_info.client_id = client_id;
        client_info.role = role;
        session->addClient(channel, client_info);

        if (role == "host") {
            channel->send(shared::JSONHelper::make_handshake_ack());
        }
        else {
            std::cout << "Guest handshake, send snapshot\n";
            channel->send(shared::JSONHelper::make_snapshot_message(session->getSnapshot()));
        }
    }

    void PerfectServer::handle_snapshot(const nlohmann::json& data) {
        const nlohmann::json j = data.at("payload");
        core::SceneSnapshot snapshot = shared::JSONHelper::deserialize_snapshot_string(j.dump());
        session->setSnapshot(snapshot);
        session->debugPrintSnapshot();
        session->setJoinable(true);
    }

    void PerfectServer::handle_session_close(const WebSocketChannelPtr& channel) {
        auto recipients = session->getClientsExcept(channel);

        std::string close_msg = shared::JSONHelper::make_session_closed_message();

        for (const auto& recipient : recipients) {
            recipient->send(close_msg);
        }

        for (const auto& [client_channel, info] : session->getConnectedClients()) {
            client_channel->close();
        }

        session->clearClients();
        stop();
        session->setActive(false);
    }

    void PerfectServer::handle_update(const WebSocketChannelPtr& channel, const std::string& type, const nlohmann::json& data, const std::string& raw_msg) {
        const nlohmann::json j = data.at("payload");
        core::SerializedObject obj = shared::JSONHelper::deserialize_object_string(j.dump());
        if (type == "update_add") {
            std::cout << "Adding object...\n";
            session->addSnapshot(obj);
        }
        else if (type == "update_edit") {
            std::cout << "Editing object...\n";
            session->editSnapshot(obj);
        }
        else if (type == "update_delete") {
            std::cout << "Deleting object...\n";
            session->deleteFromSnapshot(obj);
        }

        auto recipients = session->getClientsExcept(channel);
        for (const auto& recipient : recipients) {
            recipient->send(raw_msg);
        }
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
