#include <iostream>
#include <filesystem>

#include <rendering/GLTFModelLoader.hpp>
#include <rendering/ModelTree.hpp>
#include <rendering/MaterialManager.hpp>
#include <rendering/Model.hpp>
#include <rendering/ModelLoader.hpp>
#include <rendering/ResourceManager.hpp>
#include <rendering/CPUTexture.hpp>
#include <utils/gl/helpers.hpp>

namespace rendering {

GLTFModelLoader::GLTFModelLoader(ResourceManager& resource_manager)
    :
      resource_manager(resource_manager)
{

}

ModelTreeNode GLTFModelLoader::load_model(std::string const& model_path) {
    current_model_path = model_path;
    ModelTreeNode model_tree;
    std::cout << model_path << '\n';
    tinygltf::Model model;
    load_gltf_model(model_path, model);

    auto default_scene = model.scenes.at(model.defaultScene);

    for (auto const root_node: default_scene.nodes) {
        ModelTreeNode mt_root_node;
        load_node(mt_root_node, model, model.nodes[root_node]);
        model_tree.children.push_back(mt_root_node);
    }
    std::cout << "so apparently we successfully loaded the model tree\n";
    return model_tree;
}

void GLTFModelLoader::load_node(ModelTreeNode& mt_node, tinygltf::Model& model, tinygltf::Node& node) {

    // First things first: For each node pair we want to load the mesh.
    load_node_local_transform(mt_node, node);
    load_node_mesh(mt_node, model, node);

    for(auto const& child_node: node.children) {
        ModelTreeNode mt_child_node;
        mt_child_node.parent = &mt_node;
        load_node(mt_child_node, model, model.nodes[child_node]);
        mt_node.children.push_back(mt_child_node);
    }
}

void GLTFModelLoader::load_node_mesh(ModelTreeNode& mt_node, tinygltf::Model& model, tinygltf::Node& node) {
    if (node.mesh != -1) {
        auto const& mesh = model.meshes[node.mesh];
        for (auto gf_submesh: mesh.primitives) {
            CPUMesh submesh;
            load_submesh(mt_node, model, gf_submesh, submesh);
	    mt_node.mesh_handles.push_back(resource_manager.add_mesh_from_cpumesh(std::move(submesh)));
        }
    }
}

void GLTFModelLoader::load_submesh(ModelTreeNode& mt_node, tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh) {
    submesh.draw_mode = utils::gl::glModeFromPrimitive(primitive.mode);
    
    load_positions(model, primitive, submesh);
    load_indices(model, primitive, submesh);
    size_t norm_stride = load_normals(model, primitive, submesh);
    load_texcoord(model, primitive, submesh);
    MaterialAsset material_asset = load_materials(primitive, model);

    //submesh.normals.layout.stride = static_cast<GLsizei>(norm_stride);

	resource_manager.add_material_asset(material_asset);
    submesh.material_asset = resource_manager.add_material_asset(material_asset);

    // Next need to interleave the extra vbo
    // glBindVertexArray(0);
}

struct VBO_Slice {
    size_t offset = 0;
    std::vector<uint8_t> slice;
};

MaterialAsset GLTFModelLoader::load_materials(tinygltf::Primitive& primitive, tinygltf::Model& model) {
    std::unordered_map<int, CPUTexture> index_texture_cache;
    // will probably have to move this out as different primtives can share the same material
    //
    // If the material index is in the map, then the material refA
    //
    // If i understand correctly each mesh primitive references a material, but mutliple meshes have different primitives, which could all reference,
    // the same fucking material, and each material can also reference the same fucking texture.
    
    MaterialAsset material_asset;
    TextureAsset texture_asset;
    if (primitive.material >= 0 &&
    	primitive.material < static_cast<int>(model.materials.size())) {

    	const auto& material = model.materials[primitive.material];
    	const auto& baseTex = material.pbrMetallicRoughness.baseColorTexture;

    	//glTF convention: index == -1 means "no texture"
    	if (!index_texture_cache.contains(baseTex.index) && baseTex.index >= 0 &&
    	    baseTex.index < static_cast<int>(model.textures.size())) {
	    const auto& texture = model.textures[baseTex.index];
    	    if (texture.source >= 0 &&
    		texture.source < static_cast<int>(model.images.size())) {

    		const auto& image = model.images[texture.source];
    		if (!image.uri.empty()) {
		    //std::filesystem::path parent = p;
    		    //parent = parent.parent_path();
		    std::filesystem::path fullPath = std::filesystem::path(utils::assets::get_asset((current_model_path.parent_path() / image.uri).string()));
		    // may need to std::move the cpu_texture into the map, unsure, it will probabaly need std::moved again from the map later on as well
		    CPUTexture cpu_texture = CPUTexture(fullPath.string().c_str()); // I reall would like to avoid this to string to c_string if possible, note to future caolan to sort it out please and thank you, cheers.
		    int in = baseTex.index;
		    index_texture_cache.insert({in, cpu_texture});
		    texture_asset.cpu_texture = cpu_texture;
		    material_asset.texture_asset = texture_asset;
    		    //pr.texture = utils::Texture(fullPath.string().c_str());
    		}
    	    }
    	}
    }

    return material_asset;
}

void GLTFModelLoader::load_indices(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh) {
    if (primitive.indices >= 0) {
        const auto& iacc  = model.accessors.at(primitive.indices);
        const auto& iview = model.bufferViews.at(iacc.bufferView);
        const auto& ibuff = model.buffers.at(iview.buffer);

		const size_t component_size = utils::gl::bytesPerComponent(iacc.componentType);
    	const size_t data_size_bytes = iacc.count * component_size;

        const uint8_t* idxData = ibuff.data.data() + iview.byteOffset + iacc.byteOffset;

        submesh.indices.assign(idxData, idxData + data_size_bytes);
		submesh.index_count = static_cast<uint32_t>(iacc.count);
		submesh.index_type = utils::gl::glTypeFromComponent(iacc.componentType);

        //submesh.indexCount = static_cast<GLsizei>(iacc.count);
        //submesh.indexType  = utils::gl::glTypeFromComponent(iacc.componentType);
    } 
}

//void GLTFModelLoader::load_positions(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& cpu_mesh) {
//   auto posIt = primitive.attributes.find("POSITION");
//   if (posIt != primitive.attributes.end()) {
//   const auto& acc  = model.accessors.at(posIt->second);
//   const auto& view = model.bufferViews.at(acc.bufferView);
//   const auto& buff = model.buffers.at(view.buffer);
//
//   const size_t no_components  = utils::gl::numComponentsInType(acc.type);
//   const size_t component_size  = utils::gl::bytesPerComponent(acc.componentType);
//   const size_t stride = view.byteStride ? view.byteStride : no_components * component_size;
//
//   const uint8_t* data = reinterpret_cast<const uint8_t*> (
//   buff.data.data() + view.byteOffset + acc.byteOffset // I think maybe this could be uint8_t ?
//   );
//   // cpu_mesh.position_vbo.assign() = data;
//   // cpu_mesh.layout.attributes.push_back(VertexAttribute(
//   //         AttributeType::POSITION,
//   //         0,
//   //         utils::gl::glTypeFromComponent(acc.componentType),
//   //         no_components,
//   //         false,
//   //         (void*() 0)
//   // ));
//   }
//}

void GLTFModelLoader::load_positions(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& cpu_mesh) {
    auto posIt = primitive.attributes.find("POSITION");
    if (posIt != primitive.attributes.end()) {
        const auto& acc   = model.accessors.at(posIt->second);
        const auto& view  = model.bufferViews.at(acc.bufferView);
        const auto& buff  = model.buffers.at(view.buffer);

        const size_t no_components  = utils::gl::numComponentsInType(acc.type);
        const size_t component_size = utils::gl::bytesPerComponent(acc.componentType);
        const size_t byte_stride         = view.byteStride ? view.byteStride : (no_components * component_size);

        const uint8_t* data = buff.data.data() + view.byteOffset + acc.byteOffset;
        const size_t data_size_bytes = acc.count * byte_stride;

        // 1. Copy the raw buffer into the CPU mesh struct
        //cpu_mesh.position.vbo.assign(data, data + data_size_bytes);

        // 2. Track layout metadata needed to configure OpenGL attributes later
        cpu_mesh.vertex_count = static_cast<uint32_t>(acc.count);
        VertexAttribute attr = VertexAttribute(
            AttributeType::POSITION,
            0, // Attribute index (location = 0)
            utils::gl::glTypeFromComponent(acc.componentType),
            no_components,
            acc.normalized,
            0,
			static_cast<GLsizei>(byte_stride)
		);

		VBO_AttributePair vbo_attribute_pair;
		vbo_attribute_pair.attribute = attr;
		vbo_attribute_pair.vbo.assign(data, data + data_size_bytes);

		cpu_mesh.add_data(AttributeType::POSITION, std::move(vbo_attribute_pair));
		cpu_mesh.standalone_vbos.push_back(AttributeType::POSITION);

        //cpu_mesh.data.insert(AttributeType::POSITION, vbo_attribute_pair);S
		//cpu_mesh.position.layout.stride = static_cast<GLsizei>(stride); // Potentially need to readd this in later, using some kind of map that allows us
		// to have multiple interleaved/ singular vbo's, dynamically based on the task. More options for developers, with good defaults that just work.
    }
}

void GLTFModelLoader::load_texcoord(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& submesh) { // might want to add a string parameter for things like TEXCOORD_1 etc.
    auto texIt = primitive.attributes.find("TEXCOORD_0");
    if (texIt != primitive.attributes.end()) {
		std::cout << "Texcoord 0 loaded\n";
        const auto& acc  = model.accessors.at(texIt->second);
        const auto& view = model.bufferViews.at(acc.bufferView);
        const auto& buff = model.buffers.at(view.buffer);

        const size_t comps  = utils::gl::numComponentsInType(acc.type);
        const size_t csize  = utils::gl::bytesPerComponent(acc.componentType);
        const size_t byte_stride = view.byteStride ? view.byteStride : comps * csize;

        const unsigned char* data = buff.data.data() + view.byteOffset + acc.byteOffset;

		const size_t data_size_bytes = acc.count * byte_stride;
		auto attr = VertexAttribute(
			AttributeType::TEXCOORD_0,
			2,
			utils::gl::glTypeFromComponent(acc.componentType),
			comps,
			false,
			0,
			byte_stride
		);

		VBO_AttributePair attribute_pair;
		attribute_pair.attribute = attr;
		attribute_pair.vbo.assign(data, data + data_size_bytes);

		submesh.add_data(AttributeType::TEXCOORD_0, std::move(attribute_pair));

		submesh.interleaved_vbos.push_back(AttributeType::TEXCOORD_0);
    }
}

size_t GLTFModelLoader::load_normals(tinygltf::Model& model, tinygltf::Primitive& primitive, CPUMesh& cpu_mesh) {
    auto normIt = primitive.attributes.find("NORMAL");
    if (normIt != primitive.attributes.end()) {
        const auto& acc  = model.accessors.at(normIt->second);
        const auto& view = model.bufferViews.at(acc.bufferView);
        const auto& buff = model.buffers.at(view.buffer);

        const size_t comps  = utils::gl::numComponentsInType(acc.type);
        const size_t csize  = utils::gl::bytesPerComponent(acc.componentType);
        const size_t byte_stride = view.byteStride ? view.byteStride : comps * csize;
        const size_t offset = view.byteOffset + acc.byteOffset;

        // const unsigned char* data = buff.data.data() + offset;

        const uint8_t* data = reinterpret_cast<const uint8_t*>(buff.data.data() + offset);
        const size_t no_components  = utils::gl::numComponentsInType(acc.type);
        
		const size_t data_size_bytes = acc.count * byte_stride;

		//cpu_mesh.normals.vbo.assign(data, data + data_size_bytes); // This won't do for long as we need to interleaved the vbos.

        //cpu_mesh.normals.layout.attributes.push_back(VertexAttribute(
        auto attr = VertexAttribute(
            AttributeType::NORMAL,
            1,
            utils::gl::glTypeFromComponent(acc.componentType),
            no_components,
            false,
            0,
			byte_stride
        );

		VBO_AttributePair attribute_pair;
		attribute_pair.attribute = attr;
		attribute_pair.vbo.assign(data, data + data_size_bytes);

		cpu_mesh.interleaved_vbos.push_back(AttributeType::NORMAL);
		cpu_mesh.add_data(AttributeType::NORMAL, std::move(attribute_pair));

	return byte_stride;

        // return data;
        // glVertexAttribPointer(
        //     1,
        //     static_cast<GLint>(comps),
        //     utils::gl::glTypeFromComponent(acc.componentType), // likely GL_FLOAT
        //     acc.normalized ? GL_TRUE : GL_FALSE,
        //     static_cast<GLsizei>(stride),
        //     reinterpret_cast<void*>(0)
        // );
        // glEnableVertexAttribArray(1);
    }
}

void GLTFModelLoader::load_node_local_transform(ModelTreeNode& mt_node, const tinygltf::Node& node) {
    if (node.matrix.size() == 16) {
        mt_node.local_transform = glm::make_mat4(node.matrix.data());
    } else {
        glm::mat4 T(1.0f), R(1.0f), S(1.0f);

        if (node.translation.size() == 3) {
            T = glm::translate(glm::mat4(1.0f), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }
        if (node.rotation.size() == 4) {
            // tinygltf quaternion format: [x, y, z, w]
            glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
            R = glm::mat4_cast(q);
        }
        if (node.scale.size() == 3) {
            S = glm::scale(glm::mat4(1.0f), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }

        mt_node.local_transform = T * R * S;
    }
}

//    Model ModelLoader::load_scene(tinygltf::Scene const& scene, tinygltf::Model const& model, std::string const& p) {
//        Model m;
//        m.path = p;
//        for (auto const& n: scene.nodes) {
//            // auto const& local_transform = glm::make_mat4(model.nodes[n].matrix.data());
//            m.root_nodes.push_back(load_node(model, model.nodes[n], p));
//        }
//                     
//        return m;
//    }

//    Node ModelLoader::load_node(tinygltf::Model const& model, tinygltf::Node const& node, std::string const& p) { // We're just going to harcode the transform for now before things inherit it
//        Node my_node;
//        if (node.matrix.size() == 16) {
//            my_node.local_transform = glm::make_mat4(node.matrix.data());;
//            my_node.has_local_transform = true;
//        }
//
//        if (node.mesh != -1) {
//            auto const& mesh = model.meshes[node.mesh];
//            for (auto const& primitive: mesh.primitives) {
//                my_node.mesh.primitives.push_back(load_primitive(model, primitive, p));
//            }
//        }
//
//        for (auto const& n: node.children) {
//            my_node.children.push_back(load_node(model, model.nodes[n], p));
//        }
//        return my_node;
//    }

void GLTFModelLoader::load_gltf_model(std::string const& model_path, tinygltf::Model& model) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    const bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, utils::assets::get_asset(model_path));

    if (!warn.empty()) {
        printf("WARN: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("ERR: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Error loading model.\n.");
    }

}

tinygltf::Scene GLTFModelLoader::get_default_scene() {

}


}
