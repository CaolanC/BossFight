#pragma once

#include <rendering/Mesh.hpp>
#include <vector>

namespace rendering {
    struct Node {
        Mesh mesh;
        std::vector<Node> children;

        void add_primitive(GpuPrimitive const& pr) {
            mesh.add_primitive(pr);
        }
    };
}