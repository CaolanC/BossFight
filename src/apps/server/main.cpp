#include <hv/HttpServer.h>
#include <entt/entt.hpp>
#include <thread>
#include <concurrentqueue.h>
#include <thread>
#include <iostream>
#include <chrono>
#include <future>
#include <mutex>
#include <crossguid/guid.hpp>
#include <Server.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;

int start_server() {
    server::Server server(30000);
    server.start();

    // No need for physics ticks anymore - won't be using them. Just make thread sleep#
    // so CPU resources don't get super used up
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    };


    return 0;
}

int main() {
    return start_server();
}
