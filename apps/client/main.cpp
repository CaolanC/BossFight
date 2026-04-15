#include <string>
#include <thread>
#include <Client.hpp>

int main(int argc, char* argv[]) {

    std::string ip = "http://localhost:30000";
    bool is_host = true;

    if (argc == 2 ) {
        is_host = false;
    }

    auto client = client::Client(std::string("Perfect Client."), ip, is_host, true);

    client.run();

    return 0;
}