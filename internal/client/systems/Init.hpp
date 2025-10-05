#pragma once

#include <entt/entt.hpp>

#include <systems/MeshLoading.hpp>
#include <utils/Model.hpp>

namespace systems {
    void Init(entt::registry& r) {
        // Basically we want to be able to load a model from memory and see it
        auto& model_m = r.ctx().get<component::model_manager>().manager;
        utils::Model m = LoadModel(r, "models/fox.gltf");

        ModelHandle h = model_m.add_model(m);

        std::vector<core::ShaderSource> shader_sources = {
            core::sh_src::v3D(),
            core::sh_src::fSolid()
        };
        ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

        spawn::model(r, h, program);
    }
}
