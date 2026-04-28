#include <thread>
#include <NetBus.hpp>
#include <NetIO.hpp>
#include <entt/entt.hpp>
#include <SessionManager.hpp>
#include <Server.hpp>

using namespace server;
using namespace std::chrono_literals;

namespace server
{

    Server::Server(int port) :  port(port), io(port, bus), session_manager(bus) {

    }

    void Server::start() {
        io.start();
        thr_bus = std::jthread([this](std::stop_token st) {
                circulate_messages(st);
            }
        );
    }

    void Server::stop() {
        io.stop();
        if (sessiontemp) {
            sessiontemp->stopServer();
        }
    }

    void Server::circulate_messages(std::stop_token st) {
        while(!st.stop_requested()) {
            int dispatches = 0;
            const int max_dispatches = 120;
            NetMsg msg;
            while(dispatches < max_dispatches && bus.in_q.try_dequeue(msg)) {
                NetMsg reply = {.id=xg::newGuid(), .req_id=msg.id};
                switch (msg.type) {
                    case JoinRequest: {
                        //std::cout << msg.id << '\n';
                        reply.type=JoinReply;
                        // if (id == NULL) {
                        //     continue;
                        // }
                        break;
                    }
                    case CreateSessionRequest: {

                        xg::Guid sid;

                        bool ready;

                        int port = session_manager.find_free_port();

                        Session* existing = session_manager.getSessionByPort(port);

                        if (existing && !(existing->isActive())) {

                            existing->reset();
                            std::cout << "[Server] Starting session WS on " << port << "\n";
                            existing->setActive(true);
                            existing->startServer();
                            sid = existing->get_id();
                            session_manager.setportID(nextport, sid);

                            ready = existing->wait_until_ready(std::chrono::milliseconds(3000));
                            reply.type=CreateSessionReply;
                            reply.session_id = sid;
                            reply.ws_port = port;
                            reply.ok = ready;
                        }
                        else {

                            sid = session_manager.create();
                            sessiontemp = session_manager.getSession(sid);
                            std::cout << "[Server] Starting session WS on " << port << "\n";
                            session_manager.setportID(port, sid);
                            sessiontemp->setPSPort(port);
                            sessiontemp->setActive(true);
                            sessiontemp->startServer();

                            ready = sessiontemp->wait_until_ready(std::chrono::milliseconds(3000));
                            reply.type=CreateSessionReply;
                            reply.session_id = sid;
                            reply.ws_port = nextport;
                            reply.ok = ready;
                            nextport += 1;
                        }

                        break;
                    }

                    // case LoadSessionScene: {
                    //     if ()
                    // }

                    default: {
                            reply.type=DefaultReply;
                        break;
                    }
                }
                io.handle_reply(reply);
                dispatches++;
            }
            std::this_thread::sleep_for(5ms);
        }
    }
};

