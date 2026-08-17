#pragma once

enum class AttributeType {
    POSITION,
    NORMAL,
    TANGENT,
    TEXCOORD_0,
    TEXCOORD_1,
    COLOR_0, // Don't know if we need to dynamically load TEXOORD_N and COLOR_N, this will do for now.
    COLOR_1,
    JOINTS_0,
    JOINTS_1,
    WEIGHTS_0,
    WEIGHTS_1
};

struct VertexAttribute {
    AttributeType type = AttributeType::POSITION;
    uint32_t location = 0;
    GLenum component_type = GL_FLOAT;
    GLint num_components = 0;
    GLboolean normalized = false;
    size_t offset = 0;
};

struct VertexLayout {
    GLsizei stride = 0;
    std::vector<VertexAttribute> attributes;
};

struct GPUMesh {
    unsigned int vao;
    unsigned int position_vbo;
    unsigned int interleaved_vbo;
    unsigned int ebo;
};

struct CPUMesh {
    std::vector<uint8_t> position_vbo;
    std::vector<uint8_t> interleaved_vbo;
    VertexLayout layout;


    // Reference to material or similar.
};
