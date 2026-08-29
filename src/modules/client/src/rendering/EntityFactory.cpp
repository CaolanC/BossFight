#include <rendering/EntityFactory.hpp>
#include <rendering/ModelTree.hpp>
#include <SharedComponents.hpp>
#include <component/Core.hpp>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>

namespace rendering {

EntityFactory::EntityFactory() {}

void EntityFactory::attach_child(entt::registry& reg, entt::entity parent, entt::entity child) {
    auto& parent_hier = reg.get_or_emplace<shared::component::hierarchy>(parent);
    auto& child_hier = reg.get_or_emplace<shared::component::hierarchy>(child);

    child_hier.parent = parent;
    parent_hier.children.push_back(child);
}

entt::entity EntityFactory::create_entity_tree(entt::registry& reg, const ModelTreeNode& node, entt::entity parent) {
    auto entity = reg.create();

    // Emplace transform and hierarchy components
    reg.emplace<shared::component::position>(entity);
    reg.emplace<component::scale>(entity);
    reg.emplace<shared::component::rotation>(entity);
    reg.emplace<shared::component::transform>(entity, node.local_transform);
    reg.emplace<shared::component::hierarchy>(entity);

    // Link to parent if present
    if (parent != entt::null) {
        attach_child(reg, parent, entity);
    }

    // Attach submesh entities as children of this node
    for (MeshAssetHandle mesh_handle : node.mesh_handles) {
        auto mesh_entity = reg.create();

        component::mesh mesh_comp;
        mesh_comp.mesh_handle = mesh_handle;
        reg.emplace<component::mesh>(mesh_entity, mesh_comp);
	reg.emplace<component::scale>(entity);
        reg.emplace<shared::component::position>(mesh_entity, glm::vec3(0.0f));
        reg.emplace<shared::component::rotation>(mesh_entity, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
        reg.emplace<shared::component::transform>(mesh_entity, glm::mat4(1.0f));
        
        attach_child(reg, entity, mesh_entity);
    }

    // Recursively process child nodes
    for (const ModelTreeNode& child_node : node.children) {
        create_entity_tree(reg, child_node, entity);
    }

    return entity;
}

entt::entity EntityFactory::from_model_tree(entt::registry& reg, const ModelTreeNode& model_tree) {
    return create_entity_tree(reg, model_tree, entt::null);
}

}
