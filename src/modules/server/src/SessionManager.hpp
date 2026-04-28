#pragma once

#include <NetBus.hpp>
#include <Session.hpp>
#include <thread>
#include <memory>

namespace server
{

class SessionManager {

public:
    SessionManager(NetBus& bus);
    ~SessionManager();

    xg::Guid create();

    Session* getSession(const xg::Guid& sid);
    Session* getSessionByPort(int port);
    int find_free_port();

    void setportID(int port, xg::Guid& sid);

private:
    void start();
    void run_async();

    NetBus& bus;
    std::unordered_map<xg::Guid, std::unique_ptr<Session>> session_map;
    std::unordered_map<int, xg::Guid> port_to_sessionID;
    std::jthread thread;
    bool running = true;
};

}