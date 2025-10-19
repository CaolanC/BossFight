#pragma once

#include <rendering/Model.hpp>
#include <glm/glm.hpp>
#include <par_shapes.h>

namespace rendering {
    class ModelLoader {
    public:
        ModelLoader();

        Model load_model(std::string const& p);
        Model load_model(par_shapes_mesh_s& m);
    private:

        Model load_scene(tinygltf::Scene const& scene, tinygltf::Model const& model, std::string const& p);
        Node load_node(tinygltf::Model const& model, tinygltf::Node const& node, std::string const& p);
        GpuPrimitive load_primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string const& p);
    };
}