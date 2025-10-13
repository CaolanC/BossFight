#pragma once
#include <entt/entt.hpp>
#include <string>
#include <rendering/Model.hpp>

namespace systems {
    rendering::Model LoadModel(entt::registry& r, std::string path);
}