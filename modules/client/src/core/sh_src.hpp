#pragma once

#include <core/ShaderProgramManager.hpp>

namespace core::sh_src
{
    class v3D : public core::ShaderSource
    {
        public:
        v3D() : core::ShaderSource("shaders/v3D.glsl", ShaderType::Vertex) {

        }
    };

    class fSolid : public core::ShaderSource
    {
        public:
        fSolid() : core::ShaderSource("shaders/fSolid.glsl", ShaderType::Fragment) {

        }
    };
}