#include <thread>
#include <server/NetBus.hpp>
#include <server/NetIO.hpp>
#include <entt/entt.hpp>
#include <server/SessionManager.hpp>
#include <server/Server.hpp>

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
                        xg::Guid id = session_manager.create();

                        reply.type=CreateSessionReply;
                        break;
                    }
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

