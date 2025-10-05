//
// Created by cochrac2 on 29/09/2025.
//

#pragma once
#include <entt/entt.hpp>
#include <component/Core.hpp>

#include "component/MatRef.hpp"
#include "component/MeshRef.hpp"

namespace spawn {
    // static entt::entity freecam(entt::registry& registry) {
    //     const auto entity = registry.create();
    //     registry.emplace<component::camera>(entity);
    //     registry.emplace<component::position>(entity, glm::vec3(0, 0, 0));
    //     registry.emplace<component::rotation>(entity, glm::quat());
    //     registry.emplace<component::transform>(entity, glm::mat4(1.0f));
    //     return entity;
    // }
    static entt::entity freecam(entt::registry& r) {
        const auto e = r.create();
        r.emplace<component::camera>(e);
        r.emplace<component::position>(e, glm::vec3(0, 0, 0));
        r.emplace<component::rotation>(e, glm::angleAxis(0.0f, glm::vec3(0, 1, 0)));
        r.emplace<component::transform>(e, glm::mat4(1.0f));
        r.emplace<component::user_control>(e);
        return e;
    }

    static entt::entity triangle(entt::registry& r, xg::Guid mesh_reference, xg::Guid material_reference) {
        const auto e = r.create();
        r.emplace<component::mesh_ref>(e, mesh_reference);
        r.emplace<component::mat_ref>(e, material_reference);
        r.emplace<component::position>(e, glm::vec3(0, 0, 0));
        r.emplace<component::rotation>(e, glm::quat());
        r.emplace<component::transform>(e, glm::mat4(1.0f));
        return e;
    }

    static entt::entity model(entt::registry& r, xg::Guid model_ref, xg::Guid program_ref) {
        const auto e = r.create();
        r.emplace<component::model_ref>(e, model_ref);
        r.emplace<component::mat_ref>(e, program_ref);
        r.emplace<component::position>(e, glm::vec3(0, 0, 0));
        r.emplace<component::rotation>(e, glm::quat());
        r.emplace<component::transform>(e, glm::mat4(1.0f));
        return e;
    }

    static entt::entity raw(entt::registry& r, xg::Guid mesh_reference, xg::Guid material_reference) {
        const auto e = r.create();
        r.emplace<component::mesh_ref>(e, mesh_reference);
        r.emplace<component::mat_ref>(e, material_reference);
        r.emplace<component::position>(e, glm::vec3(0, 0, 0));
        r.emplace<component::rotation>(e, glm::quat());
        r.emplace<component::transform>(e, glm::mat4(1.0f));
        return e;
    }
};
