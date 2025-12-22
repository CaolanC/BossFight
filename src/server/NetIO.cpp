#include <server/NetIO.hpp>
#include <server/NetBus.hpp>
#include <server/NetMsg.hpp>
#include <server/ReplyPayload.hpp>
#include <thread>
#include <future>

using namespace server;
using namespace std::chrono_literals;

namespace server
{
    NetIO::NetIO(int port, NetBus& bus) : port(port), bus(bus) {

    }

    NetIO::~NetIO() {
        running = false;
        stop();
        thr_webserver.join();
    }

    void NetIO::start() {
        setup_routes();
        server = hv::HttpServer(&router);
        server.setPort(port);
        server.setThreadNum(4);
        thr_webserver = std::jthread(
            [this](std::stop_token st){
                async_run(st);
            }
        );

        running = true;
    }

    void NetIO::stop() {
        server.stop();
    }

    void NetIO::handle_reply(NetMsg msg) {

        switch (msg.type) {
            case JoinReply: {
                std::lock_guard<std::mutex> lock(req_prom_mut_join);
                auto it = waiting_join.find(msg.req_id);


                if(it == waiting_join.end()) {
                    return;
                };

                std::cout << "Client ID created for request: " << msg.req_id <<'\n';
                auto reply = server::JoinReplyPayload();
                it->second.set_value(reply);
                break;
            }

            case CreateSessionReply: {
                std::lock_guard<std::mutex> lock(req_prom_mut_create);

                auto it = waiting_create.find(msg.req_id);

                if (it == waiting_create.end()) {
                    return;
                }

                std::cout << "Server ID created for request: " << msg.req_id <<'\n';
                auto reply = server::CreateSessionReplyPayload();
                it->second.set_value(reply);

                std::cout << "Created Session: []" <<'\n';

                break;
            }

            default: {
                std::cout << "Invalid NetMsg type supplied to NetIO.";
                break;
            }
        }
    }

    void NetIO::async_run(std::stop_token st) {
        server.run();
    }

    void NetIO::setup_routes() {
        router.GET("/ping", [](HttpRequest* request, HttpResponse* response) {
            return response->String("pong :D");
        });

        router.GET("/join", [this](HttpRequest* request, HttpResponse* response) {

            std::promise<JoinReplyPayload> promise;
            auto fut = promise.get_future();
            auto id = xg::newGuid();
            {
                std::lock_guard<std::mutex> lock(req_prom_mut_join);
                waiting_join.emplace(id, std::move(promise));
            }

            bus.send_in(NetMsg{.id=id, .type=JoinRequest});

            if (fut.wait_for(3000ms) == std::future_status::ready) {
                request->headers["Connection"] = "keep-alive";
                return response->String("Joined server.");
            }
            return response->String("Server-side error joining the server.");
        });

        router.GET("/create_session", [this](HttpRequest* request, HttpResponse* response) {
            std::promise<CreateSessionReplyPayload> promise;
            auto fut = promise.get_future();
            auto id = xg::newGuid();
            {
            std::lock_guard<std::mutex> lock(req_prom_mut_join);
                waiting_create.emplace(id, std::move(promise));
            }

            bus.send_in(NetMsg{.id=id, .type=CreateSessionRequest});

            if (fut.wait_for(3000ms) == std::future_status::ready) {
                return response->String("Created session.");
            }

            return response->String("Server-side error creating the session.");
        });
    }
};