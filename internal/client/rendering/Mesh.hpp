#pragma once

#include <rendering/GpuPrimitive.hpp>
#include <vector>

namespace rendering {
    struct Mesh {
        std::vector<GpuPrimitive> primitives;
        
        void add_primitive(GpuPrimitive const& pr) {
            primitives.push_back(pr);
        }
    };
}