#include "../../internal/editor/Editor.hpp"

#include <editor/Editor.hpp>
#include <Client.hpp>

namespace editor {
    Editor::Editor(std::string const& net_addr, bool gui) : client(client::Client("Perfect.", net_addr, gui)) {

    }

    void Editor::start() {
        client.run(1920, 1080);
    };
}
