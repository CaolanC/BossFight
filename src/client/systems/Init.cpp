#include <entt/entt.hpp>
#include <systems/MeshLoading.hpp>
#include <rendering/Model.hpp>
#include <spawn/Spawn.hpp>
#include <rendering/GpuPrimitive.hpp>
#include <core/sh_src.hpp>

#include "generator/GridPlane.hpp"
#include <systems/Init.hpp>

namespace systems {
    void Init(entt::registry& r) {
        // Basically we want to be able to load a model from memory and see it
        auto& model_m = r.ctx().get<component::model_manager>().manager;
        rendering::Model m = LoadModel(r, "models/source/old_tank.gltf");

        ModelHandle h = model_m.add_model(m);

        std::vector<core::ShaderSource> shader_sources = {
            core::sh_src::v3D(),
            core::sh_src::fSolid()
        };
        ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

        auto model_e = spawn::model(r, h, program, glm::vec3(0, 0, -20), 1.0f, glm::angleAxis(glm::radians( 0.f), glm::vec3(0.f, 1.f, 0.5f)));
        r.emplace<component::debug_spin>(model_e, 0.5f);
        rendering::Model model = rendering::Model();
        rendering::Node node = rendering::Node();
        std::vector<float> const v = {
            0.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f,
            10.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f,
            10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 10.0f
        };
        node.mesh.add_primitive(rendering::pr_lines(v));
        model.root_nodes.push_back(node);
        ModelHandle const lh = model_m.add_model(model);

        spawn::model(r, lh, program);
    }
}
