#pragma once

#include <entt/entt.hpp>

#include <systems/MeshLoading.hpp>
#include <rendering/Model.hpp>
#include <spawn/Spawn.hpp>
#include <rendering/GpuPrimitive.hpp>

#include "generator/GridPlane.hpp"

#include <SceneSerializer.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>

namespace systems {
    bool Init(entt::registry& r);
    bool Init_from_file(entt::registry& r, const std::string& path, core::SceneSerializer& scene_serializer, core::SceneSnapshot& snapshot);
    bool Init_from_snapshot(entt::registry& r, core::SceneSnapshot& snapshot);
}
