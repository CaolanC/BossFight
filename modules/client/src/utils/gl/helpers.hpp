#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace utils::gl {
        void set_view_mat(const glm::mat4& view_mat, unsigned int program);
        void set_projection_mat(glm::mat4 proj_matrix, unsigned int program);
        void set_model_mat(glm::mat4 model_matrix, unsigned int program);
        void set_campos(glm::vec3 cam_pos, unsigned int program);
        size_t numComponentsInType(int type);
        size_t bytesPerComponent(int componentType);
        GLenum glTypeFromComponent(int componentType);
        GLenum glModeFromPrimitive(int mode);
};