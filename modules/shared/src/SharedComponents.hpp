#pragma once

#include <glm/glm.hpp>

namespace shared::component {
    struct position {
        glm::vec3 value;
        float pad;
    };

    using rotation = glm::quat;

    using transform = glm::mat4;
}