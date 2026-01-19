#pragma once
#include <platform/Window.hpp>

namespace editor {
    class EditorUI {
    public:

        EditorUI(Platform::Window* platform_window) : platform_window(platform_window) {

        };
        void start();
    private:
        Platform::Window* platform_window;

    };
}