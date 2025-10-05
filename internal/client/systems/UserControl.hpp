#pragma once
#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <iostream>

namespace systems {
    void UserControl(entt::registry& r) {
        const auto& kb = r.ctx().get<component::keyboard_state>();
        const auto& ms = r.ctx().get<component::mouse_state>();

        auto view = r.view<component::user_control, component::position, component::rotation>();

        const float sens = 0.05f;
        const float yaw   = glm::radians(sens * -ms.dx);
        const float pitch = glm::radians(sens * -ms.dy);

        for (auto [e, pos, rot] : view.each()) {
            rot = glm::normalize(glm::angleAxis(yaw, glm::vec3(0,1,0)) * rot);

            const glm::vec3 right = glm::normalize(rot * glm::vec3(1,0,0));
            rot = glm::normalize(glm::angleAxis(pitch, right) * rot);

            const float speed = 0.15f;
            const glm::vec3 fwd = glm::normalize(rot * glm::vec3(0,0,-1));
            const glm::vec3 strafe = glm::normalize(rot * glm::vec3(1,0,0));

            // TODO: Normalize these values and set up an input actions maps,
            if (kb.k_state[SDL_SCANCODE_W]) pos += fwd * speed;
            if (kb.k_state[SDL_SCANCODE_S]) pos -= fwd * speed;
            if (kb.k_state[SDL_SCANCODE_A]) pos -= strafe * speed;
            if (kb.k_state[SDL_SCANCODE_D]) pos += strafe * speed;
        }
    }
}
