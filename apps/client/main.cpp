#include <editor/gui.hpp>
#include <string>
#include <thread>
#include <Client.hpp>

#include "../../internal/editor/Editor.hpp"

int main(int argc, char* argv[]) {
    // auto editor = editor::Editor();
    auto client = client::Client(std::string("Perfect Client."), argv[1], true);
    std::jthread th(editor::gui);
    client.run(1920, 1080);
    return 0;
};
 