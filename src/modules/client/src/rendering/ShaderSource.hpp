#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <core/defines.hpp>

namespace rendering {

class ShaderSource
{
    public:
    ShaderSource(std::string path) : path(path){
       type = type_from_path(path);
    };

    std::filesystem::path path;
    ShaderType type;

    private:

    ShaderType type_from_path(std::filesystem::path path) {
	std::filesystem::path filename = path.filename().string();
	if (filename.empty()) {
	    // What happens if your trying to make a shader and something fails? Need to do this at some point.
	};

	char front = filename.string().front();

	if (front == 'v') { // Will probabaly want to make front uppercase to make it case insensitive later.
		return ShaderType::Vertex;
	};

	if (front == 'f') {
		return ShaderType::Fragment;
	};

	// What happens if a shader is not correctly named? Probabaly not a full crash if its in the editor mode, but this should never happen in the runtime.
    };
};

};
