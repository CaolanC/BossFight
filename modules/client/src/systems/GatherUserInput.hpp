#pragma once

#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <SDL3/SDL_keyboard.h>
#include <iostream>

namespace systems {
    void GatherUserInput(entt::registry& r);
}
