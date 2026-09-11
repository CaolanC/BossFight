#pragma once

#include <glad/glad.h>

namespace rendering {

class CPUTexture {
    public:
    CPUTexture(char const* image_path);

    GLint internal_format;
    GLenum  format;
    int width, height, nrChannels;
    unsigned char* data;
};

};
