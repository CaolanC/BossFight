#include <string>
#include <thread>
#include <Client.hpp>

int main(int argc, char* argv[]) {

    if (argc == 2 ) {
        auto client = client::Client(std::string("Perfect Client."), argv[1], false);
        client.run(1920, 1080);
    }
    else {
         auto client = client::Client(std::string("Perfect Client."), "http://localhost:30000", false);
         client.run(1920, 1080);
    }
    return 0;
}