#pragma once

#include <crossguid/guid.hpp>
#include <PerfectServer.hpp>
#include <ClientInfo.hpp>

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

private:
    xg::Guid id;
    PerfectServer ps;
    std::unordered_map<WebSocketChannelPtr, ClientInfo> connected_clients;
};

}