#pragma once

#include <rendering/ShaderSource.hpp>

#include <optional>

namespace rendering {

using ShaderProgramAssetHandle = xg::Guid;

struct ShaderProgramAsset {
	xg::Guid guid;
	std::string access_name;
	std::optional<std::string> vert_source;
	std::optional<std::string> frag_source;
	std::optional<std::string> comp_source;
	GLuint program_name;

	bool loaded = false;
};

};
