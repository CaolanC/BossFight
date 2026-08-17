#pragma once
#include <rendering/ModelTree.hpp>

namespace rendering {

class GLTFModelLoader {
    NewModelLoader(MaterialManager& material_manager);
    ModelTree load_model(std::string const& model_path);
    tinygltf::Scene get_default_scene();

    MaterialManager& material_manager;
}

}