#pragma once
#include <core/Mesh.hpp>
#include <systems/RenderUtils.hpp>
#include <string>
#include <iostream>

// namespace core { class Mesh; }

 // Helpers for sizes
        static inline size_t numComponentsInType(int type) {
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
        static inline size_t bytesPerComponent(int componentType) {
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
        static inline GLenum glTypeFromComponent(int componentType) {
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
        static inline GLenum glModeFromPrimitive(int mode) {
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
namespace utils {
    struct GpuPrimitive {
        GLuint vao = 0, vbo = 0, ebo = 0;
        GLenum mode = GL_TRIANGLES;
        GLsizei indexCount = 0;          // 0 => non-indexed
        GLenum indexType = GL_UNSIGNED_INT;
        uint32_t materialHandle = 0;     // map prim.material → your material system (later)
        GLsizei vertexCount = 0;
    };


    struct Mesh {
        std::vector<GpuPrimitive> primitives;
    };

    struct Node {
        Mesh mesh;
        std::vector<Node> children;
    };

    struct Model {
    public:
        std::vector<Node> root_nodes;
    };

    class ModelLoader {
    public:
        ModelLoader() {

        };

        Model load_model(std::string const& p) {
            tinygltf::TinyGLTF loader;
            tinygltf::Model model;
            std::string err, warn;
            const bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, systems::Utils::RenderUtils::asset(p));

            if (!warn.empty()) {
                printf("WARN: %s\n", warn.c_str());
            }

            if (!err.empty()) {
                printf("ERR: %s\n", err.c_str());
            }

            if (!ret) {
                printf("Need to look in the repo for this message lol.\n.");
            }
            auto s = model.scenes.at(model.defaultScene);
            return load_scene(s, model);
        }
    private:

        Model load_scene(tinygltf::Scene const& scene, tinygltf::Model const& model) {
            Model m;
            for (auto const& n: scene.nodes) {
                auto const& I = glm::identity<glm::mat4>();
                m.root_nodes.push_back(load_node(model, model.nodes[n], I));
            }

            return m;
        }

        Node load_node(tinygltf::Model const& model, tinygltf::Node const& node, glm::mat4 local_transform) { // We're just going to harcode the transform for now before things inherit it
            Node my_node;

            for (auto const& n: node.children) {
                my_node.children.push_back(load_node(model, model.nodes[n], local_transform));
            }

            if (node.mesh != -1) {
                auto const& mesh = model.meshes[node.mesh];
                for (auto const& primitive: mesh.primitives) {
                    my_node.mesh.primitives.push_back(load_primitive(model, primitive));
                }
            }

            return my_node;
        }

GpuPrimitive load_primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive) {
    GpuPrimitive pr{};
    pr.mode = glModeFromPrimitive(primitive.mode);

    glGenVertexArrays(1, &pr.vao);
    glBindVertexArray(pr.vao);

    auto posIt = primitive.attributes.find("POSITION");
    if (posIt != primitive.attributes.end()) {
        const auto& acc  = model.accessors.at(posIt->second);
        const auto& view = model.bufferViews.at(acc.bufferView);
        const auto& buff = model.buffers.at(view.buffer);

        const size_t comps  = numComponentsInType(acc.type);                 // usually 3
        const size_t csize  = bytesPerComponent(acc.componentType);          // usually 4
        const size_t stride = view.byteStride ? view.byteStride : comps * csize;

        const unsigned char* data = buff.data.data() + view.byteOffset + acc.byteOffset;

        glGenBuffers(1, &pr.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);
        glBufferData(GL_ARRAY_BUFFER, acc.count * stride, data, GL_STATIC_DRAW);
        pr.vertexCount = static_cast<GLsizei>(acc.count);
        glVertexAttribPointer(
            0,
            static_cast<GLint>(comps),
            glTypeFromComponent(acc.componentType),
            acc.normalized ? GL_TRUE : GL_FALSE,
            static_cast<GLsizei>(stride),
            reinterpret_cast<void*>(0)
        );
        glEnableVertexAttribArray(0);
    }

    // TODO: NORMAL, TEXCOORD_0, etc. (same pattern; may share view/stride with different byte offsets)

    if (primitive.indices >= 0) {
        const auto& iacc  = model.accessors.at(primitive.indices);
        const auto& iview = model.bufferViews.at(iacc.bufferView);
        const auto& ibuff = model.buffers.at(iview.buffer);

        const unsigned char* idxData = ibuff.data.data() + iview.byteOffset + iacc.byteOffset;

        glGenBuffers(1, &pr.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pr.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     bytesPerComponent(iacc.componentType) * iacc.count,
                     idxData,
                     GL_STATIC_DRAW);

        pr.indexCount = static_cast<GLsizei>(iacc.count);
        pr.indexType  = glTypeFromComponent(iacc.componentType);
    }

    // Per-primitive material (store handle, resolve at draw time)
    if (primitive.material >= 0) {
        pr.materialHandle = 0;
    }

    glBindVertexArray(0);
    return pr;
}


    };
}