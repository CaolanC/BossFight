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
#include <server/Server.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;

int start_game() {
    Server server(30000);
    server.start();
    const duration tick = nanoseconds(16'666'667); // Number of nanoseconds in 1/60th of a second
    const duration physics_tick = nanoseconds(0); // Need to find a good physics tick
    time_point last = steady_clock::now();

    bool running = true;

    duration accumulator = nanoseconds(0);

    int tick_count = 0;
    while(running) {
        time_point now = steady_clock::now();
        auto frame = now - last;
        last = now;
        accumulator += frame; // Accumulates 

        int updates = 0;
        const int max_updates = 5;
        while(accumulator >= tick && updates < max_updates) {
            updates++;
            accumulator -= tick;
            //std::cout << "Tick: " << tick_count  << "\n";
            tick_count++;
        }
    };

    std::this_thread::sleep_for(tick - accumulator);

    return 0;
}

int main() { // TODO: Add the ability to hurt other developers, like throw a rock at them or something and kill them. toggle dev_friendly_fire
    return start_game();
}
