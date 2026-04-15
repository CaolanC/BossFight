//
// Created by ethan on 13/03/2026.
//

#pragma once

#include <hv/WebSocketClient.h>
#include <concurrentqueue.h>

namespace client {
    class NetClient {

    public:
        bool connect(int port);
        void disconnect();
        void send(std::string msg);
        bool pollMessage(std::string& out);

        bool is_connected() const { return connected.load(); }
        bool is_connecting() const { return connecting.load(); }

    private:
        hv::WebSocketClient ws;
        moodycamel::ConcurrentQueue<std::string> messages;
        std::atomic<bool> connected{false};
        std::atomic<bool> connecting{false};
    };
}


