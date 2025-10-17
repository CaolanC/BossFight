#pragma once

#include <glad/glad.h>
#include <tiny_gltf.h>
#include <vector>

#include <utils/Texture.hpp>
#include <utils/assets/helpers.hpp>
#include <par_shapes.h>

namespace rendering {
    struct GpuPrimitive {
        GLuint vao = 0, vbo = 0, ebo = 0;
        GLenum mode = GL_TRIANGLES;
        GLsizei indexCount = 0;
        GLenum indexType = GL_UNSIGNED_INT;
        uint32_t materialHandle = 0;
        GLsizei vertexCount = 0;
        utils::Texture texture = utils::Texture(utils::assets::get_asset("textures/default_texture.jpg").c_str());

    };

    GpuPrimitive pr_lines(std::vector<float> const& vertices);
    GpuPrimitive par(par_shapes_mesh_s& m);
}
