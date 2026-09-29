#pragma once

#include <rendering/ShaderProgramAsset.hpp>
#include <rendering/ShaderSource.hpp>

#include <crossguid/guid.hpp>

#include <string>

namespace rendering {

class ShaderManager {
	public:
	ShaderManager();
	void create_shader();
	void create_program(std::string access_name, std::string vertex_path, std::string frag_path);
	void add_source(std::string path);
	ShaderSource get_source(std::string path);
	const ShaderProgramAsset& get_program(std::string access_name);
	void compile_shader(ShaderProgramAsset& shader_asset);

	std::unordered_map<std::string, ShaderProgramAsset> shader_program_assets;
	std::unordered_map<std::string, ShaderSource> shader_sources;
};

};
