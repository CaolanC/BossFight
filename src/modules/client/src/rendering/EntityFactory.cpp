#include <rendering/EntityFactory.hpp>
#include <rendering/ModelTree.hpp>
#include <SharedComponents.hpp>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>

namespace rendering {

EntityFactory::EntityFactory(entt::registry& reg) : reg(reg) {

};

void EntityFactory::from_model_tree(const ModelTreeNode& model_tree) {
    for(const ModelTreeNode& tree_node : model_tree.children) {
        // std::cout << model_tree.local_transform << '\n';
        auto pe = reg.create();
        reg.emplace<shared::component::rotation>(pe, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
        reg.emplace<shared::component::position>(pe, glm::vec3());
        reg.emplace<shared::component::transform>(pe, glm::mat4());
        //reg.emplace<shared::component::hierarchy>(pe);
        for(const CPUMesh& mesh : tree_node.meshes) {
            auto e = reg.create();
            reg.emplace<shared::component::rotation>(e, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            reg.emplace<shared::component::position>(e, glm::vec3());
            reg.emplace<shared::component::transform>(e, glm::mat4());
            //reg.emplace<shared::component::hierarchy>(e, pe);

            // add child e to pe children
        };
    };
};


};
