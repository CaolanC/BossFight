#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL_log.h>
#include <utils/gl/helpers.hpp>
#include <tiny_gltf.h>

namespace utils::gl {
    void set_view_mat(const glm::mat4& view_mat, unsigned int program) {
        glUseProgram(program);
        unsigned int view_loc = glGetUniformLocation(program, "uView");
        if (view_loc == -1) SDL_Log("WARN: uView not found"); {
            glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_mat));
        };
    }

    void set_projection_mat(glm::mat4 proj_matrix, unsigned int program) {
        glUseProgram(program);
        unsigned int projection_loc = glGetUniformLocation(program, "uProjection");
        if (projection_loc == -1) SDL_Log("WARN: uProjection not found"); {
            glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
        };
    };

    void set_model_mat(glm::mat4 model_matrix, unsigned int program) {
        glUseProgram(program);
        unsigned int model_loc = glGetUniformLocation(program, "uModel");
        if (model_loc == -1) SDL_Log("WARN: uModel not found"); {
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
        };
    };
    void set_campos(glm::vec3 cam_pos, unsigned int program) {
        glUseProgram(program);
        unsigned int cam_loc = glGetUniformLocation(program, "uCamPos");
        if (cam_loc == -1) {
            SDL_Log("WARN: uCamPos not found");
            return;
        };
        glUniform3fv(cam_loc, 1, glm::value_ptr(cam_pos));
    };

    // struct Light {
    //     glm::vec3 position;
    // };
    //
    // void set_light_pos(std::vector<Light> lights, unsigned int program) {
    //     glUseProgram(program);
    //     unsigned int light_no = 128;
    //
    //     glUniform
    // }

    size_t numComponentsInType(int type) {
        switch (type) {
            case TINYGLTF_TYPE_SCALAR: return 1;
            case TINYGLTF_TYPE_VEC2:   return 2;
            case TINYGLTF_TYPE_VEC3:   return 3;
            case TINYGLTF_TYPE_VEC4:   return 4;
            case TINYGLTF_TYPE_MAT2:   return 4;
            case TINYGLTF_TYPE_MAT3:   return 9;
            case TINYGLTF_TYPE_MAT4:   return 16;
            default: return 0;
        }
    }
    size_t bytesPerComponent(int componentType) {
        switch (componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            case TINYGLTF_COMPONENT_TYPE_BYTE:  return 1;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            case TINYGLTF_COMPONENT_TYPE_SHORT: return 2;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            case TINYGLTF_COMPONENT_TYPE_INT:
            case TINYGLTF_COMPONENT_TYPE_FLOAT: return 4;
            default: return 0;
        }
    }
    GLenum glTypeFromComponent(int componentType) {
        switch (componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  return GL_UNSIGNED_BYTE;
            case TINYGLTF_COMPONENT_TYPE_BYTE:           return GL_BYTE;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
            case TINYGLTF_COMPONENT_TYPE_SHORT:          return GL_SHORT;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   return GL_UNSIGNED_INT;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:          return GL_FLOAT;
            default: return 0;
        }
    }
    GLenum glModeFromPrimitive(int mode) {
        switch (mode) {
            case TINYGLTF_MODE_TRIANGLES:     return GL_TRIANGLES;
            case TINYGLTF_MODE_TRIANGLE_STRIP:return GL_TRIANGLE_STRIP;
            case TINYGLTF_MODE_TRIANGLE_FAN:  return GL_TRIANGLE_FAN;
            case TINYGLTF_MODE_LINE:         return GL_LINES;
            case TINYGLTF_MODE_LINE_STRIP:    return GL_LINE_STRIP;
            case TINYGLTF_MODE_POINTS:        return GL_POINTS;
            default: return GL_TRIANGLES;
        }
    }
};