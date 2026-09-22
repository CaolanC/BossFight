#pragma once

#include <rendering/ShaderSource.hpp>

namespace rendering {

using ShaderProgramAssetHandle = xg::Guid;

struct ShaderProgramAsset {
	xg::Guid guid;
	ShaderSourceHandle vert_source;
	ShaderSourceHandle frag_source;
	GLuint program_name;

	bool loaded = false;
};

};
