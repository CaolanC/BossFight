#include <systems/Debug.hpp>
#include "entt/entity/registry.hpp"
#include <component/Core.hpp>

namespace systems {
    void Debug(entt::registry& r) {
        auto view = r.view<component::debug_spin, component::rotation>();
        for (auto [entity, s, rot] : view.each()) {
            rot = glm::rotate(rot, glm::radians(glm::radians(45.0f * s.speed)), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }
}
