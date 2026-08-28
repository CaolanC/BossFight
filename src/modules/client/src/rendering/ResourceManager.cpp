#include <crossguid/guid.hpp>

#include <cstdint>
#include <string>

#include <rendering/ResourceManager.hpp>
#include <rendering/ModelTree.hpp>
#include <rendering/NewMesh.hpp>

namespace rendering {

    ResourceManager::ResourceManager() {

    }

    void ResourceManager::load_model(const std::string& model_path) {
	GLTFModelLoader model_loader(*this);
	ModelTreeNode model_tree = model_loader.load_model(model_path);
	load_model_to_gpu(model_tree);
	//model_trees.push_back(model_tree);
    };

    void ResourceManager::load_model_to_gpu(const ModelTreeNode& model_tree) {
	
    }

    MeshAssetHandle ResourceManager::add_mesh_from_cpumesh(CPUMesh cpu_mesh) {
	xg::Guid mesh_handle = xg::newGuid();
	MeshAsset mesh_asset;
	mesh_asset.cpu_mesh = std::move(cpu_mesh);
	mesh_assets.emplace(mesh_handle, std::move(mesh_asset));

	return mesh_handle;
    }

    // uint32_t ResourceManager::add_mesh() {
    //     return 0;
    // }

    // uint32_t ResourceManager::add_material() {
    //     return 0;
    // }

    // uint32_t ResourceManager::add_texture() {

    // }
}
