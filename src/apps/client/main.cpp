#include <string>
#include <thread>
#include <Client.hpp>

int main(int argc, char* argv[]) {

    // Debug client

    std::string ip = "http://localhost:30000";

    auto client = client::Client(std::string("Perfect Client."), ip, true, true, 0, client::InputMode::Client);

    client.run();

    return 0;
}