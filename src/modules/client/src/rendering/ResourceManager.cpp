#include <crossguid/guid.hpp>

#include <cstdint>
#include <string>

#include <rendering/ResourceManager.hpp>
#include <rendering/ModelTree.hpp>
#include <rendering/NewMesh.hpp>

namespace rendering {

    ResourceManager::ResourceManager() {

    }

    ModelTreeNode ResourceManager::load_model(const std::string& model_path) {
	GLTFModelLoader model_loader(*this);
	ModelTreeNode model_tree = model_loader.load_model(model_path);
	load_model_to_gpu(model_tree);

	return model_tree;
	//model_trees.push_back(model_tree);
    };

    void ResourceManager::load_model_to_gpu(const ModelTreeNode& model_tree) {
    // Start recursive traversal from the root node
    upload_node_to_gpu(model_tree);
}

void ResourceManager::upload_node_to_gpu(const ModelTreeNode& node) {
    // 1. Process all mesh handles associated with this node
    for (const MeshAssetHandle& handle : node.mesh_handles) {
        auto it = mesh_assets.find(handle);
        if (it == mesh_assets.end()) {
            continue; // Handle not found in resource map
        }

        MeshAsset& asset = it->second;

        // Skip if already uploaded to GPU
        if (asset.gpu_mesh.has_value()) {
            continue;
        }

        const CPUMesh& cpu_mesh = asset.cpu_mesh.value();
        GPUMesh gpu_mesh;

        // Generate and bind VAO
        glGenVertexArrays(1, &gpu_mesh.vao);
        glBindVertexArray(gpu_mesh.vao);

        // Upload Position / Interleaved VBO
        if (!cpu_mesh.position_vbo.empty()) {
            glGenBuffers(1, &gpu_mesh.position_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh.position_vbo);
            glBufferData(
                GL_ARRAY_BUFFER,
                cpu_mesh.position_vbo.size(),
                cpu_mesh.position_vbo.data(),
                GL_STATIC_DRAW
            );

            // Configure vertex attributes defined in layout
            for (const auto& attr : cpu_mesh.layout.attributes) {
                glVertexAttribPointer(
                    attr.location,
                    attr.num_components,
                    attr.component_type,
                    attr.normalized,
                    cpu_mesh.layout.stride,
                    reinterpret_cast<const void*>(attr.offset)
                );
                glEnableVertexAttribArray(attr.location);
            }
        }

        // Upload Index Buffer (EBO) if indices exist
        if (!cpu_mesh.indices.empty()) {
            glGenBuffers(1, &gpu_mesh.ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh.ebo);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                cpu_mesh.indices.size(),
                cpu_mesh.indices.data(),
                GL_STATIC_DRAW
            );

            gpu_mesh.count = cpu_mesh.index_count;
            gpu_mesh.index_type = cpu_mesh.index_type;
        } else {
            gpu_mesh.count = cpu_mesh.vertex_count;
            gpu_mesh.index_type = 0; // 0 indicates non-indexed draw
        }

        gpu_mesh.draw_mode = cpu_mesh.draw_mode;
        gpu_mesh.loaded = true;

        // Unbind VAO to prevent state leakage
        glBindVertexArray(0);

        // Store generated GPU handles back into the asset
        asset.gpu_mesh = gpu_mesh;
    }

    // 2. Recursively process all child nodes
    for (const ModelTreeNode& child_node : node.children) {
        upload_node_to_gpu(child_node);
    }
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
