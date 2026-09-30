#pragma once

#include <glad/glad.h>
#include <crossguid/guid.hpp>

namespace rendering {

class CPUTexture {
    public:
    CPUTexture(char const* image_path);
	
	xg::Guid guid;
    GLint internal_format;
    GLenum  format;
    int width, height, nrChannels;
    unsigned char* data;
};

};
