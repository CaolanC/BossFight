//
// Created by ethan on 07/10/2025.
//

#include "TextureManager.h"
#include <utils/Texture.h>

// Returns texture class (might be redundant)
Texture* TextureManager::make(int* objs, const char* imagePath) {
    return new Texture(objs, imagePath);
}

// Makes vector of type Texture for glTF models (may need optimization)
std::vector<Texture> TextureManager::makeglTF(tinygltf::Model model) {
    std::vector<Texture> textures;
    textures.reserve(model.images.size());
    int objCount = 1;

    for (size_t i = 0; i < model.images.size(); ++i) {
        textures.emplace_back(&objCount, model.images[i]);
    }
}
