#pragma once

#include <crossguid/guid.hpp>
#include <PerfectServer.hpp>
#include <ClientInfo.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>
#include <mutex>
#include <vector>

namespace server

{

class Session
{
public:
    Session() : id(xg::newGuid()), ps(0) {
        ps.set_session(this);
    };

    xg::Guid get_id() {
        return id;
    }

    void setPSPort(int p) {
        ps.setPort(p);
    }

    int getPort() {
        return ps.getPort();
    }

    bool wait_until_ready(std::chrono::milliseconds timeout) {
        return ps.wait_until_ready(timeout);
    }

    void startServer() { ps.start(); }
    void stopServer()  { ps.stop(); }

    void addClient(const WebSocketChannelPtr& channel, ClientInfo client_info) {
        std::lock_guard<std::mutex> lock(session_mutex);
        connected_clients[channel] = client_info;
    }

    void removeClient(const WebSocketChannelPtr& channel) {
        std::lock_guard<std::mutex> lock(session_mutex);
        connected_clients.erase(channel);
    }

    void setJoinable(bool status) {
        std::lock_guard<std::mutex> lock(session_mutex);
        guest_joinable = status;
    }

    bool isGuestJoinable() {
        std::lock_guard<std::mutex> lock(session_mutex);
        return guest_joinable;
    }

    void setSnapshot(core::SceneSnapshot snapshot) {
        std::lock_guard<std::mutex> lock(session_mutex);
        session_snapshot = snapshot;
    }

    core::SceneSnapshot getSnapshot() const {
        std::lock_guard<std::mutex> lock(session_mutex);
        return session_snapshot;
    }

    void addSnapshot(core::SerializedObject& obj) {
        std::lock_guard<std::mutex> lock(session_mutex);
        session_snapshot.insert(obj.objectID, obj);
    }

    void editSnapshot(core::SerializedObject& obj) {
        std::lock_guard<std::mutex> lock(session_mutex);
        session_snapshot.insert(obj.objectID, obj);
    }

    void deleteFromSnapshot(core::SerializedObject& obj) {
        std::lock_guard<std::mutex> lock(session_mutex);
        session_snapshot.delete_object(obj.objectID);
    }

    std::unordered_map<WebSocketChannelPtr, ClientInfo> getConnectedClients() {
        std::lock_guard<std::mutex> lock(session_mutex);
        return connected_clients;
    }

    std::vector<WebSocketChannelPtr> getClientsExcept(const WebSocketChannelPtr& avoidChannel) {
        std::vector<WebSocketChannelPtr> channels;
        std::lock_guard<std::mutex> lock(session_mutex);
        channels.reserve(connected_clients.size());

        for (const auto& [channel, info] : connected_clients) {
            if (channel != avoidChannel) {
                channels.push_back(channel);
            }
        }

        return channels;
    }

    void debugPrintSnapshot() {
        std::lock_guard<std::mutex> lock(session_mutex);
        session_snapshot.debug_print();
    }

    void applyAdd(const core::SerializedObject& obj) {
        std::lock_guard<std::mutex> lock(session_mutex);
        session_snapshot.insert(obj.objectID, obj);

    }

    void clearClients() {
        std::lock_guard<std::mutex> lock(session_mutex);
        connected_clients.clear();
    }

    void setActive(bool status) {
        std::lock_guard<std::mutex> lock(session_mutex);
        active = status;
    }

    bool isActive() {
        std::lock_guard<std::mutex> lock(session_mutex);
        return active;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(session_mutex);
        connected_clients.clear();
        session_snapshot = core::SceneSnapshot();
        guest_joinable = false;
    }

private:
    xg::Guid id;
    bool active = false;
    PerfectServer ps;
    mutable std::mutex session_mutex;
    std::unordered_map<WebSocketChannelPtr, ClientInfo> connected_clients;
    bool guest_joinable = false;
    core::SceneSnapshot session_snapshot;
};

}