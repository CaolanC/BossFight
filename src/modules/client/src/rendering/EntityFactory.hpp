#pragma once

#include <entt/entt.hpp>

#include <rendering/GLTFModelLoader.hpp>
#include <rendering/ModelTree.hpp>

namespace rendering {

class EntityFactory {
    public:
        EntityFactory();
	entt::entity from_model_tree(entt::registry& reg, const ModelTreeNode& model_tree);
	entt::entity create_entity_tree(entt::registry& reg, const ModelTreeNode& node, entt::entity parent);
	void attach_child(entt::registry& reg, entt::entity parent, entt::entity child);
    private:
};

};
