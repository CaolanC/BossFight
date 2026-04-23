#pragma once

#include <format>
#include <entt/entt.hpp>
#include <iostream>
#include <component/Core.hpp>

#include "glm/gtc/quaternion.hpp"

namespace systems {
    void Transform(entt::registry& r);
}
