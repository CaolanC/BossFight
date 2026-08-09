#pragma once

#include <vector>
#include <core/defines.hpp>
#include <core/Mesh.hpp>
#include <map>
#include <stdio.h>
#include <string>
#include <core/defines.hpp>
#include <crossguid/guid.hpp>

#include "utils/assets/helpers.hpp"

namespace core
{

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

    class Shader
    {
        public:
            Shader() {
                
            }

            void from_source(core::ShaderSource source) {

                if (glCreateShader == nullptr) {
                    std::cerr << "CRITICAL: glCreateShader pointer is NULL! Context not initialized on this thread." << std::endl;
                }

                shader = glCreateShader(source.type);
                char shader_source[2048];
                std::string s = utils::assets::get_asset(source.path);
                get_shader_source(s.c_str(), shader_source, sizeof(shader_source));
                const char* shader_src = shader_source;
                glShaderSource(shader, 1, &shader_src, NULL);
                glCompileShader(shader);

                GLint ok; char log[1024];
                glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
                if(!ok){ glGetShaderInfoLog(shader, sizeof(log), NULL, log); SDL_Log("VS: %s", log); }
            }

            unsigned int get_shader() {
                return shader;
            }

        private:
            std::vector<char> source;
            unsigned int shader;
            unsigned int program;

        void get_shader_source(const char* path, char* shader_buffer, size_t shader_buffer_length) {
            FILE* stream = fopen(path, "r");
            if (stream == NULL) {
                fprintf(stderr, "[ERROR] Could not open file: %s\n", path);
                fflush(stderr); // Forces the terminal to display the message right away
                return;
            }

            size_t bytes = fread(shader_buffer, 1, shader_buffer_length - 1, stream);
            shader_buffer[bytes] = '\0';

            fclose(stream);
        }
    };

    struct LoadedMatInfo {
        xg::Guid ref;
        unsigned int program;
    };

    class ShaderProgramManager {
    public:

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

    ShaderProgramHandle from_source_vec(const std::vector<core::ShaderSource> shader_sources) {
        
        std::vector<core::Shader> shaders;
        for (auto shader_source : shader_sources) {
            std::cout << "got to the shader loop\n";
            Shader shader;
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
