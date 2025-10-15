//
// Created by ethan on 05/10/2025.
//

#ifndef SOULJA_TEXTURE_H
#define SOULJA_TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <tiny_gltf.h>

class Texture {
public:
    unsigned int ID;

    // Supplying our own texture from file
    Texture(int* objs, const char* imagepath);

    // Getting texture for glTF model
    Texture(int* objs, const tinygltf::Image& gltfImage);

    void Bind();
};

#endif //SOULJA_T EXTURE_H