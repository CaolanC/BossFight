#pragma once

#include <glad/glad.h>
#include <tiny_gltf.h>

namespace rendering {
    struct GpuPrimitive {
        GLuint vao = 0, vbo = 0, ebo = 0;
        GLenum mode = GL_TRIANGLES;
        GLsizei indexCount = 0;
        GLenum indexType = GL_UNSIGNED_INT;
        uint32_t materialHandle = 0;
        GLsizei vertexCount = 0;
    };

    GpuPrimitive load_primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
}