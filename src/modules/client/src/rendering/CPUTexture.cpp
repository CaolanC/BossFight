#include <iostream>

#include <rendering/CPUTexture.hpp>

#include <glad/glad.h>
#include <stb_image.h>

namespace rendering {

CPUTexture::CPUTexture(char const* image_path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(image_path, &width, &height, &nrChannels, 0);

    // Determine format (3 channels = RGB, 4 channels = RGBA, et cetera)

    if (nrChannels == 3) {
        internal_format = GL_RGB;
        format = GL_RGB;
    }
    else if (nrChannels == 4) {
        internal_format = GL_RGBA;
        format = GL_RGBA;
    }
    else {
        internal_format = GL_RGB;
        format = GL_RGB;
    }

    if (!data) {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
};

};
