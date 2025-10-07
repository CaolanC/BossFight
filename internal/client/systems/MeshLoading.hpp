#pragma once
#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <string>
#include "tiny_gltf.h"
#include "glm/gtx/fast_trigonometry.hpp"
#include <rendering/Model.hpp>
#include <rendering/ModelLoader.hpp>

namespace systems {
    rendering::Model LoadModel(entt::registry& r, std::string path) {
        auto& mm = r.ctx().get<component::model_manager>();
        rendering::ModelLoader loader;
        return loader.load_model(path);
    }
}