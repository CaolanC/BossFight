#pragma once

#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <SDL3/SDL_keyboard.h>
#include <iostream>

namespace systems {
    void GatherUserInput(entt::registry& r) {
        int no_keys;
        const bool* k_state = SDL_GetKeyboardState(&no_keys);
        auto& ks = r.ctx().get<component::keyboard_state>();
        ks.k_state = k_state;

        auto& ms = r.ctx().get<component::mouse_state>();
        SDL_GetRelativeMouseState(&ms.dx, &ms.dy);
    }
}
