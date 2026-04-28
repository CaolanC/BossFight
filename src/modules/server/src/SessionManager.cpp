#include <NetBus.hpp>
#include <Session.hpp>
#include <SessionManager.hpp>

using namespace std::chrono_literals;

namespace server
{

    SessionManager::SessionManager(NetBus& bus) : bus(bus) {

    }

    SessionManager::~SessionManager() {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
    }

    xg::Guid SessionManager::create() { // Returns the session ID or NULL on error
        auto session = std::make_unique<Session>();

        auto id = session->get_id();
        session_map.emplace(id, std::move(session));
        return id;
    }

    Session* SessionManager::getSession(const xg::Guid& sid) {
        auto it = session_map.find(sid);
        if (it != session_map.end()) {
            std::cout << "Session found, returning\n";
            return it->second.get();
        }

        return nullptr;
    }

    Session* SessionManager::getSessionByPort(int port) {
        auto it = port_to_sessionID.find(port);
        if (it == port_to_sessionID.end()) {
            return nullptr;
        }

        return getSession(it->second);
    }

    int SessionManager::find_free_port() {
        for (int port = 30001; port <= 30010; ++port) {
            bool used = false;

            for (const auto& [id, session] : session_map) {
                if (session->isActive() && session->getPort() == port) {
                    used = true;
                    break;
                }
            }

            if (!used) {
                return port;
            }
        }

        return 0;
    }

    void SessionManager::setportID(int port, xg::Guid& sid) {
        port_to_sessionID[port] = sid;
    }


    void SessionManager::start() {
        thread = std::jthread([this]{run_async();});
    }

    void SessionManager::run_async() {
        running = true;
        while (running) {
            std::this_thread::sleep_for(1s);
        }
    }
};