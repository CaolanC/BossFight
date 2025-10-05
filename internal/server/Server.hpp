#pragma once

#include <thread>
#include <server/NetBus.hpp>
#include <server/NetIO.hpp>
#include <entt/entt.hpp>
#include <server/SessionManager.hpp>

using namespace server;
using namespace std::chrono_literals;

namespace server
{


class Server
{
public:
    Server(int port);

    void start();

    void stop();

private:
    NetBus bus = NetBus();
    entt::registry registry;
    NetIO io;
    int port;
    std::jthread thr_bus;   
    SessionManager session_manager;

    void circulate_messages(std::stop_token st);
};


};