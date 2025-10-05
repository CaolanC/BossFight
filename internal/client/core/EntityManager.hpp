#pragma once

#include <entt/entt.hpp>
#include <component/Core.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <format>

#include "glm/gtx/quaternion.hpp"

namespace core {
    class EntityManager {
    public:
        EntityManager() : reg(entt::registry()) {

        };
        ~EntityManager() = default;

        void update() {
            update_transform();
        }

        entt::entity spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function) {
            return spawn_function(std::ref(reg));
        }

        void move_entity(entt::entity entity, glm::vec3 position) {
            auto& pos = reg.get<component::position>(entity);
            pos += position;
        }

        void set_entity(entt::entity entity, glm::vec3 position) {
            auto&pos = reg.get<component::position>(entity);
            pos = position;
        }

        glm::mat4 get_entity_transform(entt::entity entity) {
            return reg.get<component::transform>(entity);
        }

    private:
        entt::registry reg;

        void update_transform() {
            auto view = reg.view<component::transform, component::position, component::rotation>();
            for (auto [e, transform, position, rotation]: view.each()) {
                std::cout << std::format("{}, {}, {}", position[0], position[1], position[2]) << '\n';
                transform = glm::translate(glm::identity<glm::mat4>(), position) * glm::mat4_cast(rotation);
            }
        }
    };
}
