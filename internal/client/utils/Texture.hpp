#pragma once

#include <tiny_gltf.h>
namespace utils {
    class Texture {
    public:
        unsigned int ID;

        // Supplying our own texture from file
        Texture(int* objs,  char const* imagepath);

        // Getting texture for glTF model
        Texture(int* objs, const tinygltf::Image& gltfImage);

        void Bind(int unit);
    };
}