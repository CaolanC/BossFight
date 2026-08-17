#pragma once
#include <cstdint>

namespace rendering {

    class ResourceManager {
        ResourceManager();
        uint32_t add_mesh();
        uint32_t add_texture();
        uint32_t add_material();
    };

}