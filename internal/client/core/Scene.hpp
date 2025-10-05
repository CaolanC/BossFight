#pragma once

#include <core/Object.hpp>
#include <core/cameras/CameraBase.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>

#include "EntityManager.hpp"
#include "component/Core.hpp"
#include "spawn/Spawn.hpp"
#include "systems/Render.hpp"
#include <core/MeshManager.hpp>
#include <core/sh_src.hpp>

#include <core/ShaderProgramManager.hpp>

#include <systems/GatherUserInput.hpp>
#include <systems/UserControl.hpp>
#include <systems/Transform.hpp>
#include <tuple>
#include <generator/GridPlane.hpp>

#include "systems/Init.hpp"
#include <core/ModelManager.hpp>

namespace core
{

class Scene
{
public:

     Scene(core::MeshManager const& manager, core::ModelManager const& model_manager) : mesh_manager(manager), model_manager(model_manager) {
         bootstrap();
        // spawn_default_camera();
        // spawn_triangle();
         // spawn_from_generator(generator::GridPlane);
    }

    void bootstrap() {
         int no_keys;
         const bool* k_state = SDL_GetKeyboardState(&no_keys);
         registry.ctx().emplace<component::keyboard_state>(k_state, no_keys);
         registry.ctx().emplace<component::mouse_state>(0.0f, 0.0f);
         registry.ctx().emplace<component::current_camera>(spawn_default_camera());
         registry.ctx().emplace<component::mesh_manager>(mesh_manager);
         registry.ctx().emplace<component::material_manager>(core::ShaderProgramManager());
         registry.ctx().emplace<component::model_manager>(model_manager);

         systems::Init(registry);
     }

    entt::entity spawn_default_camera() {
        return spawn(spawn::freecam);
    }

    void spawn_from_generator(std::function<std::tuple<std::vector<float>, std::vector<unsigned int>>()>const& generator) {

         auto [vertices, indices] = generator(); // 2Tuple
         core::MeshSerialiser mesh_serialiser(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
         core::Mesh mesh(vertices, indices, mesh_serialiser);

         core::MeshManager& m_manager = registry.ctx().get<component::mesh_manager>().manager;
         xg::Guid triangle_mesh_ref = m_manager.createIndexedMeshFromVertices(vertices, indices, mesh_serialiser);

         std::vector<core::ShaderSource> shader_sources = {
             core::sh_src::v3D(),
             core::sh_src::fSolid()
         };
         ShaderProgramHandle triangle_program_ref = registry.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

         spawn::raw(std::ref(registry), triangle_mesh_ref, triangle_program_ref);
     }

    void spawn_triangle() {

        std::vector<float> vertices = {
            0.0f, 0.5f, 0.0f,
           -0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };
        std::vector<unsigned int> indices = {
            0, 1, 2
        };

        core::MeshSerialiser mesh_serialiser(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        core::Mesh mesh(vertices, indices, mesh_serialiser);
         core::MeshManager& mm = registry.ctx().get<component::mesh_manager>().manager;
        xg::Guid triangle_mesh_ref = mm.createIndexedMeshFromVertices(vertices, indices, mesh_serialiser);

        std::vector<core::ShaderSource> shader_sources = {
            core::sh_src::v3D(),
            core::sh_src::fSolid()
        };
        ShaderProgramHandle triangle_program_ref = registry.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

        spawn::triangle(std::ref(registry), triangle_mesh_ref, triangle_program_ref);

    }

    entt::entity spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function) {
        return spawn_function(std::ref(registry));
    }

    void set_camera_position(glm::vec3 position) {
        auto &pos = registry.get<component::position>(registry.ctx().get<component::current_camera>().e);
        pos = position;
    }

    // entt::registry& get_registry() { // Used for run_init_scripts, might be useful to refactor this later.
    //     return std::ref(entities);
    // }

    void update() {

         auto r = std::ref(registry);
         systems::GatherUserInput(r);
         systems::UserControl(r);
         systems::Transform(r);
         systems::NewRender(r);
    }

private:
    entt::registry registry;
    core::MeshManager const& mesh_manager;
    core::ModelManager const& model_manager;
};

}
