#include <cstdint>
#include <string>

#include <rendering/ResourceManager.hpp>
#include <rendering/ModelTree.hpp>

namespace rendering {

    ResourceManager::ResourceManager() : gltf_model_loader(material_manager) {

    }

    void ResourceManager::load_model(const std::string& model_path) {
	ModelTreeNode model_tree = gltf_model_loader.load_model(model_path);
	//model_trees.push_back(model_tree);
    };

    // uint32_t ResourceManager::add_mesh() {
    //     return 0;
    // }

    // uint32_t ResourceManager::add_material() {
    //     return 0;
    // }

    // uint32_t ResourceManager::add_texture() {

    // }
}
