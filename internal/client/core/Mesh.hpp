#pragma once

#include <cstddef>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <vector>
#include <string>
#include <tiny_gltf.h>
#include <rendering/Model.hpp>
#include <core/defines.hpp>
#include <utils/gl/helpers.hpp>
namespace core
{

    struct GpuPrimitive {
        GLuint vao, vbo, ebo;
        ShaderProgramHandle material;
    };
    class MeshSerialiser
    {
    public:
        int _location;
        GLint _vec_size;
        GLenum _vec_data_type;
        GLboolean _normalized_flag;
        int _stride;
        void* _offset;

        MeshSerialiser(
            int location,
            GLint vec_size,
            GLenum vec_data_type,
            GLboolean normalized_flag,
            int stride,
            void* offset
        )
        {
            _location = location;
            _vec_size = vec_size;
            _vec_data_type = vec_data_type;
            _normalized_flag = normalized_flag;
            _stride = stride;
            _offset = offset;
        }
    };

    class Mesh
    {
    public:
        Mesh () {

        }
        void line(GLenum mode, std::vector<float>& vertices) {
            GpuPrimitive pr;
            glGenVertexArrays(1, &pr.vao);
            glBindVertexArray(pr.vao);

            glGenBuffers(1, &pr.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertices.size()*sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            gpu_primitives.push_back(pr);
        }
        Mesh(tinygltf::Model const& model, tinygltf::Mesh const& mesh) {
            for (auto const& primitive: mesh.primitives) {
                GpuPrimitive pr;
                glGenVertexArrays(1, &pr.vao);
                glBindVertexArray(pr.vao);

                auto const& pos_it = primitive.attributes.find("POSITION"); // When we're adding other attributes we will break them out into individual functions

                const tinygltf::Accessor& b_acc = model.accessors[pos_it->second];
                const tinygltf::BufferView& b_view = model.bufferViews[b_acc.bufferView];
                const tinygltf::Buffer b_buf = model.buffers[b_acc.bufferView];
                const unsigned char* vertices_data = b_buf.data.data() + b_view.byteOffset;
                glGenBuffers(1, &pr.vbo);
                glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);



                glGenBuffers(1, &pr.ebo);
                glBindBuffer(GL_ARRAY_BUFFER, pr.ebo);
                tinygltf::Accessor i_accessor = model.accessors[primitive.indices];
                tinygltf::BufferView i_view = model.bufferViews[i_accessor.bufferView];
                tinygltf::Buffer i_buf = model.buffers[i_view.buffer];
                const unsigned char* index_data = i_buf.data.data() + i_view.byteOffset; // Check if this needs the accessor offset as well
                glBufferData(pr.ebo, i_view.byteLength, index_data, GL_STATIC_DRAW);

                size_t index_count = i_accessor.count;

                glVertexAttribPointer(
                    0,
                    utils::gl::numComponentsInType(b_acc.type),
                    utils::gl::glTypeFromComponent(b_acc.componentType),
                    false, // We will definitely have to put more thought into this flag
                    b_view.byteStride ? b_acc.type * utils::gl::bytesPerComponent(b_acc.componentType) : 0,
                    (void*) b_acc.byteOffset);
                glEnableVertexAttribArray(0);
                gpu_primitives.push_back(pr);
            }
        }

        Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, core::MeshSerialiser serialiser) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            genVBO(vertices);
            genEBO(indices);
            setAttrib(serialiser);
            index_count = indices.size();
            glBindVertexArray(0);
        }

        Mesh(std::vector<float> vertices, core::MeshSerialiser serialiser) {

        }

        void draw() {

        }

        std::size_t get_index_count() {
            return index_count;
        }

        void bind_vao() const {
            glBindVertexArray(vao);
        }
    private:
        void genVBO(std::vector<float> vertices) {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        }

        void genEBO(std::vector<unsigned int> indices) {
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
        }
        void setAttrib(core::MeshSerialiser serialiser) {
            glVertexAttribPointer(serialiser._location, serialiser._vec_size, serialiser._vec_data_type, serialiser._normalized_flag, serialiser._stride, serialiser._offset);
            glEnableVertexAttribArray(serialiser._location);
        }

        unsigned int vao{}, vbo{}, ebo{};
        std::size_t index_count{};
        std::vector<GpuPrimitive> gpu_primitives;
    };

};
