#pragma once

#include <entt/entt.hpp>

#include <rendering/GLTFModelLoader.hpp>
#include <rendering/ModelTree.hpp>

namespace rendering {

class EntityFactory {
    public:
        EntityFactory(entt::registry& reg);
        void from_model_tree(const ModelTreeNode& model_tree);
    private:
    entt::registry& reg;
};

};