#pragma once

#include <thread>
#include <NetBus.hpp>
#include <NetIO.hpp>
#include <entt/entt.hpp>
#include <SessionManager.hpp>

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
    xg::Guid testtemp;
    Session* sessiontemp;

    void circulate_messages(std::stop_token st);
};


};