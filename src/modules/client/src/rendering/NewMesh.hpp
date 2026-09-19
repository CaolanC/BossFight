#pragma once

#include <cstdint>
#include <cstddef>
#include <glad/glad.h>
#include <vector>
#include <rendering/MaterialAsset.hpp>

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
	GLsizei byte_stride = 0;
};

struct VertexLayout {
    GLsizei stride = 0;
    std::vector<VertexAttribute> attributes;
};

struct GPUMesh {
    bool loaded = false;
    unsigned int vao = 0;
    unsigned int position_vbo = 0;
    unsigned int interleaved_vbo = 0;
	unsigned int texcoord_vbo = 0; // these extra vbos are temporary, just lazy tonight. Getting textures on then we chilliiiin ;)
    unsigned int ebo = 0;

    // --- Draw Metadata for OpenGL calls ---
    uint32_t count = 0;                  // vertex_count or index_count
    GLenum index_type = 0;               // 0 if non-indexed, otherwise GL_UNSIGNED_INT, etc.
    GLenum draw_mode = GL_TRIANGLES;     // GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.
	//unsigned int texture;
};

struct VBO_AttributePair {
    std::vector<uint8_t> vbo;
    VertexAttribute attribute;
};

struct CPUMesh {
	std::unordered_map<AttributeType, VBO_AttributePair> data;

    // --- Index Buffer Data ---
    std::vector<uint8_t> indices;        // Raw index data
    GLenum index_type = GL_UNSIGNED_INT; // GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, etc.

    // --- Draw Metadata ---
    uint32_t vertex_count = 0;           // Total vertices (needed for glDrawArrays)
    uint32_t index_count = 0;            // Total indices (needed for glDrawElements)

    //VertexLayout layout;
    GLenum draw_mode = GL_TRIANGLES;
    //std::optional<rendering::MaterialAssetHandle> material_asset;

	std::vector<AttributeType> standalone_vbos;
	std::vector<AttributeType> interleaved_vbos;

	// Probabaly a good idea for interleaving: Two sets or similar that define which attributes should have their own vbo and which should be interleaved.

	void add_data(AttributeType attribute_type, VBO_AttributePair attribute_pair) {
		data.emplace(attribute_type, std::move(attribute_pair));
	};
};
