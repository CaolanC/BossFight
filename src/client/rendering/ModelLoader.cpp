#include <rendering/ModelLoader.hpp>
#include <rendering/Model.hpp>
#include <glm/glm.hpp>
#include <utils/assets/helpers.hpp>
#include <utils/gl/helpers.hpp>

namespace rendering {
    ModelLoader::ModelLoader() {

    };

    Model ModelLoader::load_model(std::string const& p) {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string err, warn;
        const bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, utils::assets::get_asset(p));

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

    Model ModelLoader::load_scene(tinygltf::Scene const& scene, tinygltf::Model const& model) {
        Model m;
        for (auto const& n: scene.nodes) {
            // auto const& local_transform = glm::make_mat4(model.nodes[n].matrix.data());
            m.root_nodes.push_back(load_node(model, model.nodes[n]));
        }

        return m;
    }

    Node ModelLoader::load_node(tinygltf::Model const& model, tinygltf::Node const& node) { // We're just going to harcode the transform for now before things inherit it
        Node my_node;

        if (node.matrix.data()) {
            my_node.local_transform = glm::make_mat4(node.matrix.data());;
            my_node.has_local_transform = true;
        }

        if (node.mesh != -1) {
            auto const& mesh = model.meshes[node.mesh];
            for (auto const& primitive: mesh.primitives) {
                my_node.mesh.primitives.push_back(load_primitive(model, primitive));
            }
        }

        for (auto const& n: node.children) {
            my_node.children.push_back(load_node(model, model.nodes[n]));
        }
        return my_node;
    }
    GpuPrimitive ModelLoader::load_primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive) {
        GpuPrimitive pr{};
        pr.mode = utils::gl::glModeFromPrimitive(primitive.mode);

        glGenVertexArrays(1, &pr.vao);
        glBindVertexArray(pr.vao);

        auto normIt = primitive.attributes.find("NORMAL");
        if (normIt != primitive.attributes.end()) {
            const auto& acc  = model.accessors.at(normIt->second);
            const auto& view = model.bufferViews.at(acc.bufferView);
            const auto& buff = model.buffers.at(view.buffer);

            const size_t comps  = utils::gl::numComponentsInType(acc.type);
            const size_t csize  = utils::gl::bytesPerComponent(acc.componentType);
            const size_t stride = view.byteStride ? view.byteStride : comps * csize;
            const size_t offset = view.byteOffset + acc.byteOffset;

            const unsigned char* data = buff.data.data() + offset;

            GLuint nbo = 0;
            glGenBuffers(1, &nbo);
            glBindBuffer(GL_ARRAY_BUFFER, nbo);
            glBufferData(GL_ARRAY_BUFFER, acc.count * stride, data, GL_STATIC_DRAW);

            glVertexAttribPointer(
                1,
                static_cast<GLint>(comps),
                utils::gl::glTypeFromComponent(acc.componentType), // likely GL_FLOAT
                acc.normalized ? GL_TRUE : GL_FALSE,
                static_cast<GLsizei>(stride),
                reinterpret_cast<void*>(0)
            );
            glEnableVertexAttribArray(1);
        }

        auto posIt = primitive.attributes.find("POSITION");
        if (posIt != primitive.attributes.end()) {
            const auto& acc  = model.accessors.at(posIt->second);
            const auto& view = model.bufferViews.at(acc.bufferView);
            const auto& buff = model.buffers.at(view.buffer);

            const size_t comps  = utils::gl::numComponentsInType(acc.type);
            const size_t csize  = utils::gl::bytesPerComponent(acc.componentType);
            const size_t stride = view.byteStride ? view.byteStride : comps * csize;

            const unsigned char* data = buff.data.data() + view.byteOffset + acc.byteOffset;

            glGenBuffers(1, &pr.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);
            glBufferData(GL_ARRAY_BUFFER, acc.count * stride, data, GL_STATIC_DRAW);
            pr.vertexCount = static_cast<GLsizei>(acc.count);
            glVertexAttribPointer(
                0,
                static_cast<GLint>(comps),
                utils::gl::glTypeFromComponent(acc.componentType),
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
                         utils::gl::bytesPerComponent(iacc.componentType) * iacc.count,
                         idxData,
                         GL_STATIC_DRAW);

            pr.indexCount = static_cast<GLsizei>(iacc.count);
            pr.indexType  = utils::gl::glTypeFromComponent(iacc.componentType);
        }

        // Per-primitive material (store handle, resolve at draw time)
        if (primitive.material >= 0) {
            pr.materialHandle = 0;
        }

        glBindVertexArray(0);
        return pr;
    }
};
