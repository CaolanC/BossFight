//
// Created by ethan on 16/03/2026.
//

#pragma once
#include <string>
#include <optional>

enum class WebsocketMsg {
    Hello
};

std::string to_string(WebsocketMsg type);
std::optional<WebsocketMsg> type_from_string(const std::string& str);