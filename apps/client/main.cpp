#include <string>
#include <thread>
#include <Client.hpp>

int main(int argc, char* argv[]) {

    std::string ip = "http://localhost:30000";
    bool is_host = true;

    if (argc == 2 ) {
        is_host = false;
    }

    auto client = client::Client(std::string("Perfect Client."), ip, true, is_host, 0, true, client::InputMode::Client);
    if (client.start(ip)) {
        client.run(1920, 1080);
    }

    return 0;
}