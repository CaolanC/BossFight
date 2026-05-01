#pragma once

#include <SerializedObject.hpp>
#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <SharedComponents.hpp>

static core::SerializedObject make_test_object(){
    core::SerializedObject obj;
    obj.objectID = "obj-1";
    obj.name = "Object";
    obj.model_path = "models/test/scene.gltf";
    obj.model_ref = xg::newGuid();
    obj.position = {1.0f, 2.0f, 3.0f};
    obj.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    obj.scale = 2.0f;
    return obj;
}

static entt::entity make_controlled_entity(entt::registry& r) {
    auto e = r.create();
    r.emplace<component::user_control>(e);
    r.emplace<shared::component::position>(e, glm::vec3(0.0f));
    r.emplace<shared::component::rotation>(e, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    return e;
}

static void add_input_context(entt::registry& r) {
    r.ctx().emplace<component::keyboard_state>();
    r.ctx().emplace<component::mouse_state>();
}

static entt::entity make_transform_entity(entt::registry& r, glm::vec3 pos, float scale) {
    auto e = r.create();
    r.emplace<shared::component::position>(e, pos);
    r.emplace<shared::component::rotation>(e, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    r.emplace<shared::component::transform>(e, glm::mat4(1.0f));
    r.emplace<component::scale>(e, scale);
    return e;
}
