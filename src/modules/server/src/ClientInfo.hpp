#pragma once

#include <crossguid/guid.hpp>

struct ClientInfo {
    xg::Guid client_id;
    std::string role;
};