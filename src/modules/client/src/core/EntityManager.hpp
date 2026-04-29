#pragma once

#include <entt/entt.hpp>
#include <component/Core.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/quaternion.hpp"
#include <SharedComponents.hpp>

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
            auto& pos = reg.get<shared::component::position>(entity);
            pos.value += position;
        }

        void set_entity(entt::entity entity, glm::vec3 position) {
            auto&pos = reg.get<shared::component::position>(entity);
            pos.value = position;
        }

        glm::mat4 get_entity_transform(entt::entity entity) {
            return reg.get<shared::component::transform>(entity);
        }

    private:
        entt::registry reg;

        void update_transform() {
            auto view = reg.view<shared::component::transform, shared::component::position, shared::component::rotation>();
            for (auto [e, transform, position, rotation]: view.each()) {
                transform = glm::translate(glm::identity<glm::mat4>(), position.value) * glm::mat4_cast(rotation);
            }
        }
    };
}
