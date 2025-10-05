#pragma once

#include <format>
#include <entt/entt.hpp>
#include <iostream>
#include <component/Core.hpp>

#include "glm/gtc/quaternion.hpp"

namespace systems {
    void Transform(entt::registry& r) {
        auto view = r.view<component::transform, component::position, component::rotation>();
        for (auto [e, transform, position, rotation]: view.each()) {
            //std::cout << std::format("{}, {}, {}", position[0], position[1], position[2]) << '\n';
            transform = glm::translate(glm::identity<glm::mat4>(), position) * glm::mat4_cast(rotation);
        }
    }
}
