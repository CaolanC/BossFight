#include <rendering/ShaderManager.hpp>
#include <rendering/ShaderProgramAsset.hpp>
#include <rendering/Shader.hpp>
#include <rendering/TextureAsset.hpp>

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <string>

namespace rendering
{

ShaderManager::ShaderManager() {

};

void ShaderManager::create_program(std::string access_name, std::string vertex_path, std::string frag_path) {
	ShaderSource v_source = shader_sources.at(vertex_path);
	ShaderSource f_source = shader_sources.at(frag_path); // Needs bounds checking.
	
	ShaderProgramAsset asset;
	asset.vert_source = vertex_path;
	asset.frag_source = frag_path;
	asset.access_name = access_name;
	asset.guid = xg::newGuid();

	compile_shader(asset);

	shader_program_assets.insert({access_name, asset});
};

void ShaderManager::create_compute_program(std::string access_name, std::string compute_path) {
	ShaderSource c_source = shader_sources.at(compute_path);

	ShaderProgramAsset asset;
	asset.comp_source = compute_path;
	asset.access_name = access_name;
	asset.guid = xg::newGuid();
	
	compile_shader(asset);

	shader_program_assets.insert({access_name, asset});
};

void ShaderManager::bind_texture(std::string access_name, TextureAsset& texture_asset) {
	ShaderProgramAsset shader_asset = shader_program_assets.at(access_name);
	
	glUseProgram(shader_asset.program_name);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, texture_asset.gpu_texture.value().ID); // Remember to build this out to support other texture/image types later C!
	glUniform1i(glGetUniformLocation(shader_asset.program_name, "sd_field"), 1);

	//glBindTexture(GL_TEXTURE_3D, 0);

};

void ShaderManager::run_compute_program(std::string access_name) {
	ShaderProgramAsset asset = shader_program_assets.at(access_name);
	glUseProgram(asset.program_name);
	glDispatchCompute(8, 8, 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
};

void ShaderManager::recompile_shaders() {
	reload_sources();
	for (auto& [k, asset]: shader_program_assets) {
		compile_shader(asset);
		//glCompileShader(asset.program_name);
	}
};

void ShaderManager::add_source(std::string path) {
	shader_sources.insert({path, ShaderSource(path)});
};

void ShaderManager::reload_sources() {
	for(auto& [path, source] : shader_sources) {
		reload_source(path);
	}
};

void ShaderManager::reload_source(std::string path) {
	shader_sources.insert_or_assign(path, ShaderSource(path));
};

ShaderSource ShaderManager::get_source(std::string path) {
	shader_sources.at(path);
};


std::vector<ShaderSource> ShaderManager::get_shader_sources() {
	std::vector<ShaderSource> sources;
	for (const auto& [k, v] : shader_sources) {
		sources.push_back(v);
	};

	return sources;
}

const ShaderProgramAsset& ShaderManager::get_program(std::string access_name) {
	return shader_program_assets.at(access_name);
};

std::vector<const ShaderProgramAsset*> ShaderManager::get_programs() {
	std::vector<const ShaderProgramAsset*> programs;
	for(const auto& [k, v] : shader_program_assets) {
		programs.push_back(&v);
	}

	return programs;
};

void ShaderManager::compile_shader(ShaderProgramAsset& shader_asset) { // This feels like it could be cleaner, need to establish guid ownership formally.
	GLuint program_name = glCreateProgram();
	
	if (shader_asset.vert_source.has_value()) {
		Shader vert_shader;
		vert_shader.from_source(shader_sources.at(shader_asset.vert_source.value()));
        	glAttachShader(program_name, vert_shader.get_shader());
	}

	if (shader_asset.frag_source.has_value()) {
		Shader frag_shader;
		frag_shader.from_source(shader_sources.at(shader_asset.frag_source.value()));
        	glAttachShader(program_name, frag_shader.get_shader());
	}

	if (shader_asset.comp_source.has_value()) {
		Shader comp_shader;
		comp_shader.from_source(shader_sources.at(shader_asset.comp_source.value()));
		glAttachShader(program_name, comp_shader.get_shader());
	}

        glLinkProgram(program_name);

        GLint ok = GL_FALSE;
        glGetProgramiv(program_name, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[2048];
            glGetProgramInfoLog(program_name, sizeof log, nullptr, log);
            SDL_Log("Link error: %s", log);
	}

	shader_asset.program_name = program_name;
    };
};
#ifdef CHEESE
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

#endif  
