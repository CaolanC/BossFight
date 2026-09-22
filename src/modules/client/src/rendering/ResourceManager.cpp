#include <crossguid/guid.hpp>

#include <cstdint>
#include <string>

#include <rendering/ResourceManager.hpp>
#include <rendering/ModelTree.hpp>
#include <rendering/NewMesh.hpp>
#include <rendering/CPUTexture.hpp>
#include <rendering/GPUTexture.hpp>
#include <Pak.hpp>

namespace rendering {

    ResourceManager::ResourceManager() {

    }

	void ResourceManager::write_pak_file() {
		pak::PakFile pak_file("Assets1.pak");

		for(auto& mesh_A : mesh_assets) { // Obviously need to check if the cpu_mesh actually exists, handing this over to you future C.
			pak_file.add_mesh(mesh_A.second.cpu_mesh.value());
		};

		for(auto& texture_A : texture_assets) { // Obviously need to check if the cpu_mesh actually exists, handing this over to you future C.
			pak_file.add_texture(texture_A.second.cpu_texture.value());
		};

		for(auto& material_A : material_assets) { // Obviously need to check if the cpu_mesh actually exists, handing this over to you future C.
			pak_file.add_material(material_A.second);
		};

		pak_file.close();
		
	};

    void ResourceManager::init() {
		

		ShaderSource vert_shader_source = ShaderSource("shaders/v3D.glsl");
		ShaderSource frag_shader_source = ShaderSource("shaders/fBasicLighting.glsl");

        xg::Guid vsh_src_guid = xg::newGuid();
        xg::Guid fsh_src_guid = xg::newGuid();

        shader_sources.insert({vsh_src_guid, vert_shader_source});
        shader_sources.insert({fsh_src_guid, frag_shader_source});

		ShaderProgramAsset shader_program_asset;

		shader_program_asset.vert_source = vsh_src_guid;
		shader_program_asset.frag_source = fsh_src_guid;


		default_shader_program_handle = compile_shader(shader_program_asset);
		shader_program_manager.init();
		
    }

    ModelTreeNode ResourceManager::load_model(const std::string& model_path) {
		GLTFModelLoader model_loader(*this);
		ModelTreeNode model_tree = model_loader.load_model(model_path);
		load_model_to_gpu(model_tree);

		return model_tree;
    };

    void ResourceManager::load_model_to_gpu(const ModelTreeNode& model_tree) {
    	// Start recursive traversal from the root node
    	upload_node_to_gpu(model_tree);
	}

	void ResourceManager::upload_node_to_gpu(const ModelTreeNode& node) {
	    // 1. Process all mesh handles associated with this node
	    for (const MeshAssetHandle& handle : node.mesh_handles) {
			upload_texture_to_gpu(material_assets.at(node.mesh_material_map.at(handle)).base_color_texture_handle.value()); // Will obviously need to get rid of this and the mesh material map in model tree
	        auto it = mesh_assets.find(handle);
	        if (it == mesh_assets.end()) {
	            continue; // Handle not found in resource map
	        }
	
	        MeshAsset& asset = it->second;
	
	        // Skip if already uploaded to GPU
	        if (asset.gpu_mesh.has_value()) {
	            continue;
	        }
	
	        const CPUMesh& cpu_mesh = asset.cpu_mesh.value();
	        GPUMesh gpu_mesh;
	
	        // Generate and bind VAO
	        glGenVertexArrays(1, &gpu_mesh.vao);
	        glBindVertexArray(gpu_mesh.vao);
	
	        // Upload Position / Interleaved VBO
			const VBO_AttributePair& position_pair = cpu_mesh.data.at(AttributeType::POSITION);
	        if (!position_pair.vbo.empty()) {
	            glGenBuffers(1, &gpu_mesh.position_vbo);
	            glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh.position_vbo);
	            glBufferData(
	                GL_ARRAY_BUFFER,
	                position_pair.vbo.size(),
	                position_pair.vbo.data(),
	                GL_STATIC_DRAW
	            );
	
				const auto& attr = position_pair.attribute;
	
	            //// Configure vertex attributes defined in layout
	            //for (const auto& attr : position_pair.attributes) {
	            glVertexAttribPointer(
	                attr.location,
	                attr.num_components,
	                attr.component_type,
	                attr.normalized,
	                attr.byte_stride,
	                reinterpret_cast<const void*>(attr.offset)
	            );
	            glEnableVertexAttribArray(attr.location);
	            //}
	        }
	
			
			const VBO_AttributePair& normal_pair = cpu_mesh.data.at(AttributeType::NORMAL);
	        if (!normal_pair.vbo.empty()) {
	            glGenBuffers(1, &gpu_mesh.interleaved_vbo);
	            glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh.interleaved_vbo);
	            glBufferData(
	                GL_ARRAY_BUFFER,
	                normal_pair.vbo.size(),
	                normal_pair.vbo.data(),
	                GL_STATIC_DRAW
	            );
	
	            // Configure vertex attributes defined in layout
				const auto& attr = normal_pair.attribute;
	            glVertexAttribPointer(
	                attr.location,
	                attr.num_components,
	                attr.component_type,
	                attr.normalized,
	                attr.byte_stride,
	                reinterpret_cast<const void*>(attr.offset)
	            );
	            glEnableVertexAttribArray(attr.location);
	        } // Right, next need to interleave the cpu buffers :)
	
			const void* offset = 0;
			//for(auto& attr_type : cpu_mesh.interleaved_vbos) {
				//const VBO_AttributePair& pair = cpu_mesh.data.at(attr_type);
				//glGenBuffers(1, &gpu_mesh.interleaved_vbo;
				//glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh.interleaved_vbo);
				//glBufferData(
				//	GL_ARRAY_BUFFER,
				//	pair.vbo.size(),
				//	pair.vbo.data(),
				//	GL_STATIC_DRAW
				//);
				
				//const auto& attr = pair.attribute;
				//glVertexAttribPointer(
				//	attr.location,
				//	attr.num_components
				//);
				
			//}
			//const VBO_AttributePair& texcoord_pair = cpu_mesh.data.at(AttributeType::TEXCOORD_0);
			//const auto& attr = texcoord_pair.attribute;
			
			const VBO_AttributePair& texcoord_pair = cpu_mesh.data.at(AttributeType::TEXCOORD_0);
	        if (!texcoord_pair.vbo.empty()) {
	            glGenBuffers(1, &gpu_mesh.texcoord_vbo);
	            glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh.texcoord_vbo);
	            glBufferData(
	                GL_ARRAY_BUFFER,
	                texcoord_pair.vbo.size(),
	                texcoord_pair.vbo.data(),
	                GL_STATIC_DRAW
	            );
	
	            // Configure vertex attributes defined in layout
				const auto& attr = texcoord_pair.attribute;
	            glVertexAttribPointer(
	                attr.location,
	                attr.num_components,
	                attr.component_type,
	                attr.normalized,
	                attr.byte_stride,
	                reinterpret_cast<const void*>(attr.offset)
	            );
	            glEnableVertexAttribArray(attr.location);
	        }
	
			// Upload Texture
		//	GPUTexture gpu_texture;
		//	if (cpu_mesh.material_asset.has_value()) {
		//		const MaterialAssetHandle& material_asset_handle = cpu_mesh.material_asset.value();
		//		const MaterialAsset& material_asset = material_assets.at(material_asset_handle);
		//		const CPUTexture& cpu_texture = material_asset.texture_asset.cpu_texture.value();
		//		
		//		glGenTextures(1, &gpu_mesh.texture); // Need to change this later to keep the asset system intact, does for now
		//		glBindTexture(GL_TEXTURE_2D, gpu_mesh.texture);
	
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		if (cpu_texture.data) {
		//			glTexImage2D(GL_TEXTURE_2D, 0, cpu_texture.format, cpu_texture.width, cpu_texture.height, 0, cpu_texture.format, GL_UNSIGNED_BYTE, cpu_texture.data);
	    //			glGenerateMipmap(GL_TEXTURE_2D);
	    //		}
	
		//		
	
		//		//asset.material_asset = 
		//		
		//	}
	
	        // Upload Index Buffer (EBO) if indices exist
	        if (!cpu_mesh.indices.empty()) {
	            glGenBuffers(1, &gpu_mesh.ebo);
	            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh.ebo);
	            glBufferData(
	                GL_ELEMENT_ARRAY_BUFFER,
	                cpu_mesh.indices.size(),
	                cpu_mesh.indices.data(),
	                GL_STATIC_DRAW
	            );
	
	            gpu_mesh.count = cpu_mesh.index_count;
	            gpu_mesh.index_type = cpu_mesh.index_type;
	        } else {
	            gpu_mesh.count = cpu_mesh.vertex_count;
	            gpu_mesh.index_type = 0; // 0 indicates non-indexed draw
	        }
	
	        gpu_mesh.draw_mode = cpu_mesh.draw_mode;
	        gpu_mesh.loaded = true;
	
	        // Unbind VAO to prevent state leakage
	        glBindVertexArray(0);
	
	        // Store generated GPU handles back into the asset
	        asset.gpu_mesh = gpu_mesh;
			
	    }
	
	    // 2. Recursively process all child nodes
	    for (const ModelTreeNode& child_node : node.children) {
	        upload_node_to_gpu(child_node);
	    }
	}

    MeshAssetHandle ResourceManager::add_mesh_from_cpumesh(CPUMesh cpu_mesh) {
	xg::Guid mesh_handle = xg::newGuid();
	MeshAsset mesh_asset;
	mesh_asset.cpu_mesh = std::move(cpu_mesh);
	mesh_assets.emplace(mesh_handle, std::move(mesh_asset));

	return mesh_handle;
    }

	MaterialAssetHandle ResourceManager::add_material_asset(MaterialAsset material_asset) {
		MaterialAssetHandle material_handle = xg::newGuid();
		material_assets.emplace(material_handle, material_asset);

		return material_handle;
	};

	TextureAssetHandle ResourceManager::add_texture_asset(TextureAsset texture_asset) {
		TextureAssetHandle texture_handle = xg::newGuid();
		texture_assets.emplace(texture_handle, texture_asset);

		return texture_handle;
	};

    // uint32_t ResourceManager::add_mesh() {
    //     return 0;
    // }

    // uint32_t ResourceManager::add_material() {
    //     return 0;
    // }

    // uint32_t ResourceManager::add_texture() {

    // }

	void ResourceManager::upload_texture_to_gpu(TextureAssetHandle handle)
	{
    	auto it = texture_assets.find(handle);

    	if (it == texture_assets.end()) {
    	    return;
		}

    	TextureAsset& asset = it->second;

    	if (asset.gpu_texture.has_value()) {

    	    return;
		}

    	if (!asset.cpu_texture.has_value()) {
    	    return;
		}

    	const CPUTexture& cpu = *asset.cpu_texture;

    	GPUTexture gpu;

    	glGenTextures(1, &gpu.ID);
    	glBindTexture(GL_TEXTURE_2D, gpu.ID);

    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    	glTexParameteri(
    	    GL_TEXTURE_2D,
    	    GL_TEXTURE_MIN_FILTER,
    	    GL_LINEAR_MIPMAP_LINEAR
    	);

    	glTexParameteri(
    	    GL_TEXTURE_2D,
    	    GL_TEXTURE_MAG_FILTER,
    	    GL_LINEAR
    	);

    	glTexImage2D(
    	    GL_TEXTURE_2D,
    	    0,
    	    cpu.internal_format,
    	    cpu.width,
    	    cpu.height,
    	    0,
    	    cpu.format,
    	    GL_UNSIGNED_BYTE,
    	    cpu.data
    	);

    	glGenerateMipmap(GL_TEXTURE_2D);

    	glBindTexture(GL_TEXTURE_2D, 0);

    	gpu.loaded = true;
    	asset.gpu_texture = gpu;
	}

    xg::Guid ResourceManager::compile_shader(ShaderProgramAsset shader_asset) { // This feels like it could be cleaner, need to establish guid ownership formally.

		rendering::Shader vert_shader;
		vert_shader.from_source(shader_sources.at(shader_asset.vert_source));

		rendering::Shader frag_shader;
		frag_shader.from_source(shader_sources.at(shader_asset.frag_source));

        shader_asset.program_name = glCreateProgram();
		GLuint program_name = shader_asset.program_name;

        glAttachShader(program_name, vert_shader.get_shader());
        glAttachShader(program_name, frag_shader.get_shader());

        glLinkProgram(program_name);

        GLint ok = GL_FALSE;
        glGetProgramiv(program_name, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[2048];
            glGetProgramInfoLog(program_name, sizeof log, nullptr, log);
            SDL_Log("Link error: %s", log);
		}
		xg::Guid sh_pr_guid = xg::newGuid();
		shader_asset.guid = sh_pr_guid;
		shader_program_assets.insert({sh_pr_guid, shader_asset});

		shaders.insert({xg::newGuid(), vert_shader});
		shaders.insert({xg::newGuid(), frag_shader});

        return sh_pr_guid; 
    };

}
