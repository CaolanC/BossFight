#pragma once

#include <NetBus.hpp>
#include <thread>
#include <hv/HttpServer.h>
#include <hv/WebSocketServer.h>
#include <future>
#include <ReplyPayload.hpp>

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
    std::unordered_map<xg::Guid, std::promise<JoinReplyPayload>> waiting_join;
    std::unordered_map<xg::Guid, std::promise<CreateSessionReplyPayload>> waiting_create;
    std::mutex req_prom_mut_join;
    std::mutex req_prom_mut_create;
    void async_run(std::stop_token st);

    void setup_routes();
};


}