#pragma once

#include <Client.hpp>

namespace editor {
    class Editor {
    public:
        Editor(std::string const& net_addr, bool gui);

        void start();
    private:
        client::Client client;
    };
}