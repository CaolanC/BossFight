//
// Created by ethan on 07/10/2025.
//

#ifndef SOULJA_TEXTUREMANAGER_H
#define SOULJA_TEXTUREMANAGER_H
#include <utils/Texture.hpp>
#include <tiny_gltf.h>

class TextureManager {
public:
    utils::Texture* make(int* objs, const char* imagePath);

    std::vector<utils::Texture> makeglTF(tinygltf::Model model);
};

#endif //SOULJA_TEXTUREMANAGER_H