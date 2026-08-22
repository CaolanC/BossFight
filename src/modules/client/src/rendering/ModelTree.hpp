#pragma once

#include <rendering/NewMesh.hpp>
#include <glm/glm.hpp>
#include <string>

struct baseColorTexture {
    int index;
    int texcoord;
};

struct pbrMetallicRoughness {
    baseColorTexture base_color_texture;
    glm::vec4 baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
};

struct ModelTreeMaterial {
    std::string name;
    pbrMetallicRoughness pbr_metallic_roughness;

};

struct CPUMaterial {

};

struct ModelTreeNode {
    ModelTreeNode* parent = nullptr;
    std::string name;
    glm::mat4 local_transform = {1.0f}; // It's local transform is at the origin, I think this works?
    
    std::vector<ModelTreeNode> children;
    std::vector<CPUMesh> meshes;
    CPUMaterial material;
    
    // Each node can reference a mesh, material, texture ?
    // Children
    // Parent
    // Local transform
};

class ModelTree {
    public:
    ModelTree();
    private:
    ModelTreeNode root_node = ModelTreeNode();
};