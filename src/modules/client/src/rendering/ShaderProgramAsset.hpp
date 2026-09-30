#pragma once

#include <rendering/ShaderSource.hpp>

namespace rendering {

using ShaderProgramAssetHandle = xg::Guid;

struct ShaderProgramAsset {
	xg::Guid guid;
	std::string access_name;
	std::string vert_source;
	std::string frag_source;
	GLuint program_name;

	bool loaded = false;
};

};
