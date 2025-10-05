#pragma once

#include <server/NetBus.hpp>
#include <thread>
#include <hv/HttpServer.h>
#include <future>
#include <server/ReplyPayload.hpp>

namespace server

{

class NetIO
{
public:
    NetIO(int port, NetBus& bus);
    ~NetIO();

    void start();

    void stop();

    void handle_reply(NetMsg msg);

private:
    std::jthread thr_reply_dispatch, thr_webserver;
    hv::HttpServer server;
    hv::HttpService router;
    int listenfd;
    int port;
    bool running = false;
    NetBus& bus;
    std::unordered_map<xg::Guid, std::promise<JoinReplyPayload>> waiting_joiners;
    std::mutex req_prom_mut;

    void async_run(std::stop_token st);

    void setup_routes();
};


}