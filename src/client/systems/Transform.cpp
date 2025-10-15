#include <format>
#include <entt/entt.hpp>
#include <iostream>
#include <component/Core.hpp>
#include "glm/gtc/quaternion.hpp"
#include <systems/Transform.hpp>

namespace systems {
    void Transform(entt::registry& r) {
        auto view = r.view<component::transform, component::position, component::rotation, component::scale>();
        for (auto [e, transform, position, rotation, scale]: view.each()) {
            transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale.s));;
        }
    }
}
