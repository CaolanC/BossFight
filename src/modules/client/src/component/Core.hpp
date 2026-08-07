#pragma once

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>
#include <core/MeshManager.hpp>
#include <core/ShaderProgramManager.hpp>
#include <core/ModelManager.hpp>
#include <array>
#include <SDL3/SDL.h>

#include <string>

namespace component
{

    // Holds components for ENTT registry.

    struct camera {

    };

    struct entity_name {
        std::string name;
    };

    struct free_movement_camera {

    };

    struct camera_attachment {

    };

    struct game_input {

    };

    struct keyboard_state {
        std::array<bool, SDL_SCANCODE_COUNT> down{};
    };

    struct mouse_state {
        float x = 0.0f;
        float y = 0.0f;
        float dx = 0.0f;
        float dy = 0.0f;
        std::array<bool, 8> down{};
        bool inside_viewport = false;
    };

    struct user_control {};

    // struct mesh_manager {
    //     core::MeshManager manager;
    // };

    // struct material_manager {
    //     core::ShaderProgramManager manager;
    // };

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

    struct name {
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
