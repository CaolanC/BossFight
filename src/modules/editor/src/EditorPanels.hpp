#pragma once
#include <EditorApp.hpp>

namespace gui {
    class EditorPanels {
    public:
        static void draw_tools(AppContext& app);
        static void draw_viewport(AppContext& app);
        static void draw_right(AppContext& app);
        static void draw_bottom(AppContext& app);
    };
}


