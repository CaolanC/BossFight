#pragma once
#include <EditorApp.hpp>

namespace gui {
    class EditorPanels {
    public:
        void draw_tools(AppContext& app);
        void draw_viewport(AppContext& app);
        void draw_right(AppContext& app);
        void draw_bottom(AppContext& app);
    };
}


