#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace shared::component {
    struct position {
        glm::vec3 value;
        float pad;
    };

    struct hierarchy {
        entt::entity parent{entt::null};
        std::vector<entt::entity> children;
    };

    using rotation = glm::quat;

    using transform = glm::mat4;
}
