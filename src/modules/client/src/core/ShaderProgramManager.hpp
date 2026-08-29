#pragma once

#include <vector>
#include <core/defines.hpp>
#include <core/Mesh.hpp>
#include <map>
#include <stdio.h>
#include <string>
#include <core/defines.hpp>
#include <crossguid/guid.hpp>
#include <core/sh_src.hpp>
#include <rendering/ShaderSource.hpp>

#include "utils/assets/helpers.hpp"

namespace core
{
    struct LoadedMatInfo {
        xg::Guid ref;
        unsigned int program;
    };

    class ShaderProgramManager {
    public:

    ShaderProgramHandle default_material;

    ShaderProgramManager() {};
    
    void init_default_material() {
	std::vector<rendering::ShaderSource> shader_sources = {
       	    core::sh_src::v3D(),
            core::sh_src::fSolid()
        };

        default_material = from_source_vec(shader_sources);
    }

    std::vector<LoadedMatInfo> get_loaded_materials() const {
        std::vector<LoadedMatInfo> out;
        out.reserve(program_map.size());

        for (const auto& [ref, program] : program_map) {
            LoadedMatInfo info;
            info.ref= ref;
            info.program = program;
            out.push_back(info);
            // auto it = model_to_path.find(ref);
            // if (it != model_to_path.end()) {
            //     info.model_path = it->second;
            //     out.push_back(info);
            // }

        }

        return out;
    }

    ShaderProgramHandle from_source_vec(const std::vector<rendering::ShaderSource> shader_sources) {
        
        std::vector<rendering::Shader> shaders;
        for (auto shader_source : shader_sources) {
            std::cout << "got to the shader loop\n";
	    rendering::Shader shader;
            shader.from_source(shader_source);

            shaders.push_back(shader);
        }

        unsigned int program = glCreateProgram();
        for (auto shader : shaders) {
            glAttachShader(program, shader.get_shader());
        }

        glLinkProgram(program);

        GLint ok = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[2048];
            glGetProgramInfoLog(program, sizeof log, nullptr, log);
            SDL_Log("Link error: %s", log);
        }

        auto id = xg::newGuid();
        program_map.insert({id, program});
        return id;
    };

    unsigned int get_program(ShaderProgramHandle id) {
        return program_map.at(id);
    }

    private:
    std::map<ShaderProgramHandle, unsigned int> program_map;
};

}
