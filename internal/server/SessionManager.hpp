#pragma once

#include <server/NetBus.hpp>
#include <server/Session.hpp>
#include <thread>

namespace server
{

class SessionManager {

public:
    SessionManager(NetBus& bus);
    ~SessionManager();

    xg::Guid create();

private:
    void start();
    void run_async();

    NetBus& bus;
    std::unordered_map<xg::Guid, Session> session_map;
    std::jthread thread;
    bool running = true;
};

}