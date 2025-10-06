#pragma once
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "SDL3/SDL_log.h"
#include <string>
#include <iostream>

namespace systems::Utils {
    class RenderUtils {
    public:
        static void set_view_mat(const glm::mat4& view_mat, unsigned int program) {
            glUseProgram(program);
            unsigned int view_loc = glGetUniformLocation(program, "uView");
            if (view_loc == -1) SDL_Log("WARN: uView not found"); {
                glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_mat));
            };
        }

        static void set_projection_mat(glm::mat4 proj_matrix, unsigned int program) {
            glUseProgram(program);
            unsigned int projection_loc = glGetUniformLocation(program, "uProjection");
            if (projection_loc == -1) SDL_Log("WARN: uProjection not found"); {
                glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
            };
        };

        static void set_model_mat(glm::mat4 model_matrix, unsigned int program) {
            glUseProgram(program);
            unsigned int model_loc = glGetUniformLocation(program, "uModel");
            if (model_loc == -1) SDL_Log("WARN: uModel not found"); {
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            };
        };
        static void set_campos(glm::vec3 cam_pos, unsigned int program) {
            glUseProgram(program);
            unsigned int cam_loc = glGetUniformLocation(program, "uCamPos");
            if (cam_loc == -1) SDL_Log("WARN: CamPos not found"); {
                glUniform3fv(cam_loc, 1, glm::value_ptr(cam_pos));
            };
        };

        static std::string asset(std::string const& path) {
            auto s = SLJA_ASSETS_DIR + path;
            std::cout << s << '\n';
            return s;
        }
    };
}
