#pragma once

#include <rendering/Mesh.hpp>
#include <vector>

namespace rendering {
    struct Node {
        Mesh mesh;
        std::vector<Node> children;
    };
}