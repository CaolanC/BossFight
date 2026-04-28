#pragma once
#include <string>
#include <crossguid/guid.hpp>

namespace core {
    struct LoadedModelInfo {
        xg::Guid model_ref;
        std::string model_path;
    };
}