#pragma once

#include <entt/entt.hpp>

#include <systems/MeshLoading.hpp>
#include <rendering/Model.hpp>
#include <spawn/Spawn.hpp>
#include <rendering/GpuPrimitive.hpp>

#include "generator/GridPlane.hpp"

namespace systems {
    bool Init(entt::registry& r);
}
