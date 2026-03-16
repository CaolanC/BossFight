//
// Created by ethan on 16/03/2026.
//

#include <WebsocketMsg.hpp>

std::string to_string(WebsocketMsg type) {
    switch (type) {
        case WebsocketMsg::Hello: return "HELLO";
    }
    return "UNKNOWN";
}

std::optional<WebsocketMsg> type_from_string(const std::string& str) {
    if (str == "HELLO") { return WebsocketMsg::Hello; }
    return std::nullopt;
}
