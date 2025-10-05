#pragma once
#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <string>
#include <systems/RenderUtils.hpp>
#include "tiny_gltf.h"
#include "glm/gtx/fast_trigonometry.hpp"
#include <utils/Model.hpp>

namespace systems {
    utils::Model LoadModel(entt::registry& r, std::string path) {
        auto& mm = r.ctx().get<component::model_manager>();
        utils::ModelLoader loader;
        return loader.load_model(path);
    }
}