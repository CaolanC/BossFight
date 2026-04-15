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