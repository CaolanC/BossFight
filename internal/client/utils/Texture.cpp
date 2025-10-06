//
// Created by ethan on 05/10/2025.
//
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"
#include <glad/glad.h>
#include <iostream>



Texture::Texture(int* objs, const char* imagepath) {
    glGenTextures(*objs, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(imagepath, &width, &height, &nrChannels, 0);

    // Determine format (3 channels = RGB, 4 channels = RGBA, et cetera)

    GLint internalFormat;
    GLenum format;
    if (nrChannels == 3) {
        internalFormat = GL_RGB;
        format = GL_RGB;
    }
    else if (nrChannels == 4) {
        internalFormat = GL_RGBA;
        format = GL_RGBA;
    }
    else {
        internalFormat = GL_RGB;
        format = GL_RGB;
    }

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, ID);
}
