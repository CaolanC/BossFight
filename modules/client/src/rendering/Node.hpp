#pragma once

#include <rendering/Mesh.hpp>
#include <vector>
#include <glm/glm.hpp>

namespace rendering {
    struct Node {
        Mesh mesh;
        std::vector<Node> children;
        glm::mat4 local_transform;
        bool has_local_transform = false;

        void add_primitive(GpuPrimitive const& pr) {
            mesh.add_primitive(pr);
        }
    };
}