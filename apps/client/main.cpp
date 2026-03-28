#include <string>
#include <thread>
#include <Client.hpp>

int main(int argc, char* argv[]) {

    if (argc == 2 ) {
        auto client = client::Client(std::string("Perfect Client."), argv[1], false, true);
        if (client.start(argv[1])) {
            client.run(1920, 1080);
            auto client2 = client::Client(std::string("Perfect Client."), argv[1], false, false);
            if (client2.start(argv[1])) {
                client2.run(1920, 1080);
            }
        }
    }
    else {
        auto client = client::Client(std::string("Perfect Client."), "http://localhost:30000", false, true);
        if (client.start("http://localhost:30000")) {
            client.run(1920, 1080);
            auto client2 = client::Client(std::string("Perfect Client."), argv[1], false, false);
            if (client2.start("http://localhost:30000")) {
                client2.run(1920, 1080);
            }
        }
    }
    return 0;
}