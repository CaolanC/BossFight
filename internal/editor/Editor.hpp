#pragma once

#include <Client.hpp>
#include <platform/Window.hpp>
#include "EditorUI.hpp"

namespace editor {
    class Editor {
    public:
        Editor(std::string const& net_addr, bool gui) : window(Platform::Window("Perfect.", 1920, 1080)), editor_ui(&window) {

        };

        void start();
    private:
        // client::Client client;
        editor::EditorUI editor_ui;
        Platform::Window window;
    };
}
