#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include <rendering/GLTFModelLoader.hpp>
#include <rendering/ModelTree.hpp>
#include <rendering/MaterialManager.hpp>
#include <rendering/MeshAsset.hpp>
#include <rendering/MaterialAsset.hpp>
#include <rendering/NewMesh.hpp>
#include <core/ShaderProgramManager.hpp>

namespace rendering {

    class GLTFModelLoader;

    class ResourceManager {
        public:
        ResourceManager();
	ModelTreeNode load_model(const std::string& model_path);
	void load_model_to_gpu(const ModelTreeNode& model_tree);
	void upload_node_to_gpu(const ModelTreeNode& node);
	MeshAssetHandle add_mesh_from_cpumesh(CPUMesh cpu_mesh);

	
        private:

	MaterialManager material_manager;
	std::vector<ModelTreeNode> model_trees;
	
	std::unordered_map<MeshAssetHandle, MeshAsset> mesh_assets;
	std::unordered_map<MaterialAssetHandle, MaterialAsset> material_asset;
	core::ShaderProgramManager shader_program_manager;
        
        // uint32_t add_mesh();
        // uint32_t add_texture();
        // uint32_t add_material();
        // Probably want something more like vector of ModelTree nodes representing meshes
    };

}
