#include <rendering/ShaderManager.hpp>
#include <rendering/ShaderProgramAsset.hpp>
#include <rendering/Shader.hpp>

#include <SDL3/SDL.h>

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

void ShaderManager::add_source(std::string path) {
	shader_sources.insert({path, ShaderSource(path)});
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

	Shader vert_shader;
	vert_shader.from_source(shader_sources.at(shader_asset.vert_source));

	Shader frag_shader;
	frag_shader.from_source(shader_sources.at(shader_asset.frag_source));

//        shader_asset.program_name = glCreateProgram();
//	GLuint program_name = shader_asset.program_name;
	GLuint program_name = glCreateProgram();

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
