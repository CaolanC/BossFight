#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <string>
#include "tiny_gltf.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/fast_trigonometry.hpp"
#include <rendering/Model.hpp>
#include <rendering/ModelLoader.hpp>
#include <systems/MeshLoading.hpp>
#include <par_shapes.h>

namespace systems {
    rendering::Model LoadModel(entt::registry& r, std::string path) {
        auto& mm = r.ctx().get<component::model_manager>();
        rendering::ModelLoader loader;
        return loader.load_model(path);
    }

    rendering::Model LoadModel(entt::registry& r, par_shapes_mesh_s& par_m) {
        auto& mm = r.ctx().get<component::model_manager>();
        rendering::ModelLoader loader;
        return loader.load_model(par_m);
    }
}