#pragma once

#include <entt/entt.hpp>
#include <core/MeshManager.hpp>
#include <core/Mesh.hpp>
#include "component/MeshRef.hpp"
#include <core/ShaderProgramManager.hpp>
#include <component/Core.hpp>
#include <utils/gl/helpers.hpp>

namespace systems {
    void NewRender(entt::registry& reg);
}