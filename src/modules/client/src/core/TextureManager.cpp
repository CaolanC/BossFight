#include "TextureManager.hpp"
#include <utils/Texture.hpp>

// Returns texture class (might be redundant)
utils::Texture* TextureManager::make(int* objs, const char* imagePath) {
    return new utils::Texture(imagePath);
}

// Makes vector of type Texture for glTF models (may need optimization)
std::vector<utils::Texture> TextureManager::makeglTF(tinygltf::Model model) {
    std::vector<utils::Texture> textures;
    textures.reserve(model.images.size());
    int objCount = 1;

    for (size_t i = 0; i < model.images.size(); ++i) {
        textures.emplace_back(&objCount, model.images[i]);
    }

    return textures;
}
