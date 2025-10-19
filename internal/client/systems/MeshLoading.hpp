#pragma once
#include <entt/entt.hpp>
#include <string>
#include <rendering/Model.hpp>
#include <par_shapes.h>

namespace systems {
    rendering::Model LoadModel(entt::registry& r, std::string path);
    rendering::Model LoadModel(entt::registry& r, par_shapes_mesh_s& par_m);
}