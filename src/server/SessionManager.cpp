#include <server/NetBus.hpp>
#include <server/Session.hpp>
#include <server/SessionManager.hpp>

using namespace std::chrono_literals;

namespace server
{

    SessionManager::SessionManager(NetBus& bus) : bus(bus) {

    }

    SessionManager::~SessionManager() {
        running = false;
        thread.join();
    }

    xg::Guid SessionManager::create() { // Returns the session ID or NULL on error
        Session session;
        auto id = session.get_id();
        session_map.emplace(id, session);
        return id;
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