#pragma once

#include <glad/glad.h>
#include <rendering/ShaderSource.hpp>
//#include <SDL/SDL3.h>

namespace rendering {

	using ShaderHandle = xg::Guid;

    class Shader
    {
        public:
            Shader() {
                
            }

            void from_source(rendering::ShaderSource source) {

                //if (glCreateShader == nullptr) {
                //    std::cerr << "CRITICAL: glCreateShader pointer is NULL! Context not initialized on this thread." << std::endl;
                //} // TBH this should never ever ever happen. So I comment it. But I'm a dumbass, this is handy for dev. Hmm. If all else fails uncomment this bs.

                shader = glCreateShader(source.type);
                char shader_source[32412];
                //std::string s = utils::assets::get_asset(source.path);
                
                std::string s = source.full_path;
                get_shader_source(s.c_str(), shader_source, sizeof(shader_source));
                const char* shader_src = shader_source;
                //const char* shader_src = source.text;
               
                glShaderSource(shader, 1, &shader_src, NULL);
                glCompileShader(shader);

                GLint ok; char log[1024];
                glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
                //if(!ok){ glGetShaderInfoLog(shader, sizeof(log), NULL, log); SDL_Log("VS: %s", log); }
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

}
