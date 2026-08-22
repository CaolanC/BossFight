#pragma once
#include <cstdint>
#include <string>

#include <rendering/GLTFModelLoader.hpp>
#include <rendering/ModelTree.hpp>
#include <rendering/MaterialManager.hpp>

namespace rendering {

    class ResourceManager {
        public:
        ResourceManager();
	void load_model(const std::string& model_path);
        private:

	MaterialManager material_manager;
	GLTFModelLoader gltf_model_loader;
	std::vector<ModelTreeNode> model_trees;
        
        // uint32_t add_mesh();
        // uint32_t add_texture();
        // uint32_t add_material();
        // Probably want something more like vector of ModelTree nodes representing meshes
    };

}
