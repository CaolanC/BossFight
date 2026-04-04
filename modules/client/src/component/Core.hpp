#pragma once

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>
#include <core/MeshManager.hpp>
#include <core/ShaderProgramManager.hpp>
#include <core/ModelManager.hpp>

namespace component
{

    struct camera {

    };

    struct free_movement_camera {

    };

    struct camera_attachment {

    };

    struct game_input {

    };

    struct keyboard_state {
        const bool* k_state;
        int no_keys;
    };

    struct mouse_state {
        float dx, dy;
    };

    struct user_control {};

    struct mesh_manager {
        core::MeshManager manager;
    };

    struct material_manager {
        core::ShaderProgramManager manager;
    };

    struct current_camera {
        entt::entity e;
    };

    struct models_to_load {
        std::vector<std::string> paths;
    };

    struct model_manager {
        core::ModelManager manager;
    };

    struct model_ref {
        xg::Guid id;
    };

    struct object_id {
        std::string value;
    };

    struct model_path {
        std::string value;
    };

    struct scale {
        float s;
    };

    struct debug_spin {
        float speed = 1.0f;
    };

}
