#pragma once

#include <core/ShaderProgramManager.hpp>
#include <rendering/ShaderSource.hpp>
#include <rendering/Shader.hpp>

namespace core::sh_src
{
    class v3D : public rendering::ShaderSource
    {
        public:
        v3D() : rendering::ShaderSource("shaders/v3D.glsl", ShaderType::Vertex) {

        }
    };

    class fSolid : public rendering::ShaderSource
    {
        public:
        fSolid() : rendering::ShaderSource("shaders/fSolid.glsl", ShaderType::Fragment) {

        }
    };

    // Extra shader for platform

    class fPlatform : public rendering::ShaderSource {
    public:
        fPlatform() : rendering::ShaderSource("shaders/fPlatform.glsl", ShaderType::Fragment) {

        }
    };
};
