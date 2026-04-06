#include <entt/entt.hpp>
#include <systems/MeshLoading.hpp>
#include <rendering/Model.hpp>
#include <spawn/Spawn.hpp>
#include <rendering/GpuPrimitive.hpp>
#include <core/sh_src.hpp>
#include <par_shapes.h>

#include "generator/GridPlane.hpp"
#include <systems/Init.hpp>
#include <utils/Texture.hpp>
#include <utils/assets/helpers.hpp>

namespace systems {
    bool Init(entt::registry& r) {
        // Basically we want to be able to load a model from memory and see it
        auto& model_m = r.ctx().get<component::model_manager>().manager;
        rendering::Model m = LoadModel(r, "models/carl_johnson_cj/scene.gltf");

        ModelHandle h = model_m.add_model(m);

        std::vector<core::ShaderSource> shader_sources = {
            core::sh_src::v3D(),
            core::sh_src::fSolid()
        };
        ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

        auto model_e = spawn::model(r, h, program, "models/carl_johnson_cj/scene.gltf", glm::vec3(0, 0, 0), 1.0f, glm::angleAxis(glm::radians( 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        r.emplace<component::debug_spin>(model_e, 0.2f);
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

        par_shapes_mesh* par_m = par_shapes_create_subdivided_sphere(4);
        par_shapes_mesh* k_par_m = par_shapes_create_trefoil_knot(100, 100, 1);
        rendering::Model sphere_m = LoadModel(r, *par_m);
        auto const sphere_h = model_m.add_model(sphere_m);
        spawn::model(r, sphere_h, program, "basicsphere", glm::vec3(2, 0, 0));

        return true;
    }

    bool Init_from_file(entt::registry& r, const std::string& path, core::SceneSerializer& scene_serializer) {
        // Same code as above but changed to work from a file
        auto& model_m = r.ctx().get<component::model_manager>().manager;

        std::vector<core::ShaderSource> shader_sources = {
            core::sh_src::v3D(),
            core::sh_src::fSolid()
        };
        ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

        core::SceneSnapshot snapshot = core::SceneSnapshot();

        bool ok = scene_serializer.load_from_file(path, snapshot);
        snapshot.debug_print();
        if (ok) {
            for (const auto& [key, obj] : snapshot.getmap()) {
                if (obj.model_path == "basicsphere") {
                    par_shapes_mesh* par_m = par_shapes_create_subdivided_sphere(4);
                    par_shapes_mesh* k_par_m = par_shapes_create_trefoil_knot(100, 100, 1);
                    rendering::Model sphere_m = LoadModel(r, *par_m);
                    auto const sphere_h = model_m.add_model(sphere_m, obj.model_ref);
                    spawn::model(r, sphere_h, program, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID);
                }
                else if (obj.model_path == "none") {
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
                    ModelHandle const lh = model_m.add_model(model, obj.model_ref);
                    spawn::model(r, lh, program, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID);
                }
                else {
                    rendering::Model m = LoadModel(r, obj.model_path);
                    ModelHandle h = model_m.add_model(m, obj.model_ref);
                    auto model_e = spawn::model(r, h, program, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID);
                    r.emplace<component::debug_spin>(model_e, 0.2f);
                }

            }
            return true;
        }

        return false;
    }

}
