#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <iostream>
#include <systems/UserControl.hpp>
#include <SharedComponents.hpp>
#include <chrono>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace systems {
    void UserControl(entt::registry& r) {
        // 1. Guard against uninitialized context objects
        auto* kb = r.ctx().find<component::keyboard_state>();
        auto* ms = r.ctx().find<component::mouse_state>();

        if (!kb || !ms) {
            return; // Exit safely if input states aren't populated yet
        }

        static auto last_time = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last_time).count();
        last_time = now;

        dt = std::min(dt, 0.05f); // Clamp delta time spike

        // 2. Fetch components (including transform so it stays updated for rendering)
        auto view = r.view<component::user_control, shared::component::position, shared::component::rotation, shared::component::transform>();

        const float sens = 0.002f; // Adjusted sensitivity for raw mouse deltas

        for (auto [e, pos, rot, tr] : view.each()) {
            // 1. Mouse Rotation
            if (ms->dx != 0 || ms->dy != 0) {
                float yaw = -ms->dx * sens;
                float pitch = -ms->dy * sens;

                glm::quat yaw_quad = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
                rot = glm::normalize(yaw_quad * rot); // rot used directly (no .value)

                glm::vec3 right = rot * glm::vec3(1.0f, 0.0f, 0.0f);
                if (glm::length(right) > 0.0001f) {
                    right = glm::normalize(right);
                    glm::quat pitch_quad = glm::angleAxis(pitch, right);
                    rot = glm::normalize(pitch_quad * rot); // rot used directly (no .value)
                }
            }

            // 2. Direction Vectors
            glm::vec3 fwd = rot * glm::vec3(0.0f, 0.0f, -1.0f);   // rot used directly
            glm::vec3 strafe = rot * glm::vec3(1.0f, 0.0f, 0.0f); // rot used directly

            if (glm::length(fwd) > 0.0001f) fwd = glm::normalize(fwd);
            if (glm::length(strafe) > 0.0001f) strafe = glm::normalize(strafe);

            const float speed = 6.0f * dt;

            // pos is a struct, so pos.value stays as-is
            if (kb->down[SDL_SCANCODE_W]) pos.value += fwd * speed;
            if (kb->down[SDL_SCANCODE_S]) pos.value -= fwd * speed;
            if (kb->down[SDL_SCANCODE_A]) pos.value -= strafe * speed;
            if (kb->down[SDL_SCANCODE_D]) pos.value += strafe * speed;

            // 3. Matrix Transformations
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos.value);
            glm::mat4 rotation_mat = glm::mat4_cast(rot); // rot used directly (no .value)
            
            tr = translation * rotation_mat; // tr is glm::mat4 directly (no .value)
        }
    }
}