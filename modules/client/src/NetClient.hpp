//
// Created by ethan on 13/03/2026.
//

#pragma once

#include <hv/WebSocketClient.h>

namespace client {
    class NetClient {

    public:
        bool connect(int port);
        void disconnect();
        void send();

    private:
        hv::WebSocketClient ws;
    };
}


