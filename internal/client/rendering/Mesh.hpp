#pragma once

#include <rendering/GpuPrimitive.hpp>
#include <vector>

namespace rendering {
    struct Mesh {
        std::vector<GpuPrimitive> primitives;
    };
}