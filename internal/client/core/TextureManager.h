//
// Created by ethan on 07/10/2025.
//

#ifndef SOULJA_TEXTUREMANAGER_H
#define SOULJA_TEXTUREMANAGER_H
#include <utils/Texture.h>
#include <tiny_gltf.h>

class TextureManager {
public:
    Texture* make(int* objs, const char* imagePath);

    std::vector<Texture> makeglTF(tinygltf::Model model);
};

#endif //SOULJA_TEXTUREMANAGER_H