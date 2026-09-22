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
    class ShaderProgramManager {
    public:

    std::unordered_map<ShaderProgramHandle, unsigned int> shader_programs;

    ShaderProgramHandle default_program;

    ShaderProgramManager() {
    };
    
    void init() {
	std::vector<rendering::ShaderSource> shader_sources = {
	    rendering::ShaderSource("shaders/v3D.glsl"),
	    rendering::ShaderSource("shaders/fBasicLighting.glsl")
        };

        default_program = from_source_vec(shader_sources);
//from_source_vec(shader_sources);
    }

    void shader_path_to_type(std::string shader_path) {
	
    } 

    ShaderProgramHandle from_source_vec(const std::vector<rendering::ShaderSource>& shader_sources) {
            
		std::vector<rendering::Shader> shaders;
        auto id = xg::newGuid();

        for (const auto& shader_source: shader_sources) {
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

        program_map.insert({id, program});
		source_map.insert({id, shader_sources});
		

        return id;
    };

    unsigned int get_program(ShaderProgramHandle id) {
        return program_map.at(id);
    }

    std::map<ShaderProgramHandle, unsigned int> program_map;
	std::map<ShaderProgramHandle, std::vector<rendering::ShaderSource>> source_map;
};

}
