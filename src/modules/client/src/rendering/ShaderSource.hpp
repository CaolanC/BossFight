#pragma once

#include <glad/glad.h>

namespace rendering {

class ShaderSource
{
    public:
    ShaderSource(std::string _path, ShaderType _type) {
       path = _path;
       type = _type;
    }

    std::string path;
    ShaderType type;
};

};
