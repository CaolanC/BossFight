//
// Created by ethan on 13/03/2026.
//

#include <NetClient.hpp>
#include <string>

namespace client
{

    // Some code taken from libhv documentation (see: WebSockets Client example on github repo)

    bool NetClient::connect(std::string& host, int port) {

        if (connected.load() || connecting.load()) {
            return false;
        }

        std::string url = "ws://" + host + ":" + std::to_string(port);
        std::cout << "[CLIENT] Attempting to connect to: " << url << "\n";
        connecting = true;

        ws.onopen = [this]() {
            std::cout << "onopen client\n";
            connected = true;
            connecting = false;
        };

        ws.onmessage = [this](const std::string& msg) {
            messages.enqueue(msg);
        };

        ws.onclose = [this]() {
            std::cout << "onclose client\n";
            connected = false;
            connecting = false;
        };

        ws.open(url.c_str());

        return true;

    }

    void NetClient::disconnect() {
        ws.close();
        connected = false;
        connecting = false;
    }

    void NetClient::send(std::string msg) {
        if (connected.load()) {
            ws.send(msg);
        }
    }

    bool NetClient::pollMessage(std::string& out){
        return messages.try_dequeue(out);
    }
}