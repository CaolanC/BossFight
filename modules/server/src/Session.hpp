#pragma once

#include <crossguid/guid.hpp>
#include <PerfectServer.hpp>
#include <ClientInfo.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>

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

    bool wait_until_ready(std::chrono::milliseconds timeout) {
        return ps.wait_until_ready(timeout);
    }

    void startServer() { ps.start(); }
    void stopServer()  { ps.stop(); }

    void addClient(const WebSocketChannelPtr& channel, ClientInfo client_info) {
        connected_clients[channel] = client_info;
    }

    void setJoinable(bool status) {
        guest_joinable = status;
    }

    bool isGuestJoinable() {
        return guest_joinable;
    }

    void setSnapshot(core::SceneSnapshot snapshot) {
        session_snapshot = snapshot;
    }

    core::SceneSnapshot getSnapshot() {
        return session_snapshot;
    }

    void addOrEditSnapshot(core::SerializedObject& obj) {
        session_snapshot.insert(obj.objectID, obj);
    }

    void deleteFromSnapshot(core::SerializedObject& obj) {
        session_snapshot.delete_object(obj.objectID);
    }

    std::unordered_map<WebSocketChannelPtr, ClientInfo> getConnectedClients() {
        return connected_clients;
    }

private:
    xg::Guid id;
    PerfectServer ps;
    std::unordered_map<WebSocketChannelPtr, ClientInfo> connected_clients;
    bool guest_joinable = false;
    core::SceneSnapshot session_snapshot;
};

}