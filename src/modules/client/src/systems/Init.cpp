#include <entt/entt.hpp>
#include <systems/MeshLoading.hpp>
#include <rendering/Model.hpp>
#include <spawn/Spawn.hpp>
#include <rendering/GpuPrimitive.hpp>
#include <core/sh_src.hpp>
#include <par_shapes.h>
#include <glm/glm.hpp>

#include "generator/GridPlane.hpp"
#include <systems/Init.hpp>
#include <utils/Texture.hpp>
#include <utils/assets/helpers.hpp>

namespace systems {

    // bool Local_files_exist(core::SceneSnapshot& snapshot) {
    //     for (const auto& [key, obj] : snapshot.getmap()) {
    //         if (!(utils::assets::asset_exists(obj.model_path))) {
    //             std::cout << "Missing asset for object " << obj.objectID << ": " << obj.model_path << "\n";
    //             return false;
    //         }

    //     }
    //     return true;
    // }

    // bool Init_spawn_platform(entt::registry& r) {
    //     auto& model_m = r.ctx().get<component::model_manager>().manager;

    //     std::vector<core::ShaderSource> shader_sources = {
    //         core::sh_src::v3D(),
    //         core::sh_src::fPlatform()
    //     };
    //     ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

    //     par_shapes_mesh* platform_mesh = par_shapes_create_cube();

    //     par_shapes_scale(platform_mesh, 300.0f, 0.0f, 300.0f);
    //     par_shapes_translate(platform_mesh, -150.0f, -0.06f, -150.0f);

    //     rendering::Model platform_model = systems::LoadModel(r, *platform_mesh);

    //     xg::Guid platform_ref = xg::newGuid();
    //     model_m.add_model(platform_model, platform_ref);

    //     const auto e = r.create();

    //     r.emplace<component::model_ref>(e, platform_ref);
    //     r.emplace<component::mat_ref>(e, program);
    //     r.emplace<shared::component::position>(e, glm::vec3(0.0f, 0.0f, 0.0f));
    //     r.emplace<shared::component::rotation>(e, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    //     r.emplace<shared::component::transform>(e, glm::mat4(1.0f));
    //     r.emplace<component::scale>(e, 1.0f);

    //     par_shapes_free_mesh(platform_mesh);

    //     return true;
    // }

    // bool Init_from_file(entt::registry& r, const std::string& path, core::SceneSerializer& scene_serializer, core::SceneSnapshot& snapshot, std::unordered_map<std::string, entt::entity>& object_lookup) {
    //     // Same code as above but changed to work from a file
    //     auto& model_m = r.ctx().get<component::model_manager>().manager;

    //     std::vector<core::ShaderSource> shader_sources = {
    //         core::sh_src::v3D(),
    //         core::sh_src::fSolid()
    //     };
    //     ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

    //     bool ok = scene_serializer.load_from_file(path, snapshot);
    //     // snapshot.debug_print();
    //     if (ok) {
    //         if (!(Local_files_exist(snapshot))) {
    //             return false;
    //         }
    //         for (const auto& [key, obj] : snapshot.getmap()) {

    //             if (!(model_m.check_ref(obj.model_ref))) {
    //                 rendering::Model m = LoadModel(r, obj.model_path);
    //                 model_m.add_model(m, obj.model_path, obj.model_ref);
    //             }

    //             auto model_e = spawn::model(r, obj.model_ref, program, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID, obj.name);
    //             object_lookup[obj.objectID] = model_e;

    //         }

    //         return true;
    //     }

    //     return false;
    // }

    // bool Init_from_snapshot(entt::registry& r, core::SceneSnapshot& snapshot, std::unordered_map<std::string, entt::entity>& object_lookup) {

    //     auto& model_m = r.ctx().get<component::model_manager>().manager;

    //     std::vector<core::ShaderSource> shader_sources = {
    //         core::sh_src::v3D(),
    //         core::sh_src::fSolid()
    //     };
    //     ShaderProgramHandle program = r.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

    //     for (const auto& [key, obj] : snapshot.getmap()) {

    //         if (!(model_m.check_ref(obj.model_ref))) {
    //             rendering::Model m = LoadModel(r, obj.model_path);
    //             model_m.add_model(m, obj.model_path, obj.model_ref);
    //         }
    //         auto model_e = spawn::model(r, obj.model_ref, program, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID, obj.name);
    //         object_lookup[obj.objectID] = model_e;

    //     }

    //     return true;

    // }

}
