#pragma once
#include <rendering/ModelTree.hpp>
#include <rendering/MaterialManager.hpp>
#include <rendering/NewMesh.hpp>

#include <tiny_gltf.h>

namespace rendering {

class GLTFModelLoader {
    public:
    GLTFModelLoader(MaterialManager& material_manager); // We need a CPU side way to store textures/materials, so put some thought into this
    ModelTreeNode load_model(std::string const& model_path);
    tinygltf::Scene get_default_scene();

    void load_gltf_model(std::string const& model_path, tinygltf::Model& model);
    void load_node(ModelTreeNode& mt_node, tinygltf::Model& model, tinygltf::Node& node);
    void load_node_local_transform(ModelTreeNode& mt_node, const tinygltf::Node& node);
    void load_node_mesh(ModelTreeNode& mt_node, tinygltf::Model& model, tinygltf::Node& node);
    void load_submesh(ModelTreeNode& mt_node, tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh);
    void load_positions(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh);
    void load_normals(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh);
    void load_texcoord(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh);
    void load_indices(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh);
    void load_materials(tinygltf::Primitive& primitive);
    MaterialManager& material_manager;
};

}