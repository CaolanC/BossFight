#pragma once

#include <rendering/Model.hpp>
#include <glm/glm.hpp>

namespace rendering {
    class ModelLoader {
    public:
        ModelLoader();

        Model load_model(std::string const& p);
    private:

        Model load_scene(tinygltf::Scene const& scene, tinygltf::Model const& model);
        Node load_node(tinygltf::Model const& model, tinygltf::Node const& node);
        GpuPrimitive load_primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
    };
}