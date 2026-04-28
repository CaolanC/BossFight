//
// Created by ethan on 13/03/2026.
//

#include <NetClient.hpp>
#include <string>

namespace client
{

    bool NetClient::connect(std::string& host, int port) {

        if (connected.load() || connecting.load()) {
            return false;
        }

        // std::string url = "ws://localhost:" + std::to_string(port);
        std::string url = "ws://" + host + ":" + std::to_string(port);
        connecting = true;

        ws.onopen = [this]() {
            std::cout << "onopen client\n";
            connected = true;
            connecting = false;
        };

        ws.onmessage = [this](const std::string& msg) {
            std::cout << "repeat msg from server";
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