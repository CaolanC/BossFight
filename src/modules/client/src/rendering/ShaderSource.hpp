#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <core/defines.hpp>

#include <utils/assets/helpers.hpp>

#include <iostream>

namespace rendering {

using ShaderSourceHandle = xg::Guid;

class ShaderSource
{
    public:
    ShaderSource(std::string path) : path(path){
    	type = type_from_path(path);
 		full_path = utils::assets::get_asset(path);
		char shsrc[32000];
		get_shader_source(full_path.c_str(), shsrc, sizeof(shsrc));
		text = shsrc;
    };

    std::filesystem::path path;
    ShaderType type;
	const char* text;
	std::string full_path;

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

    void get_shader_source(const char* path, char* shader_buffer, size_t shader_buffer_length) {
        FILE* stream = fopen(path, "r");
        if (stream == NULL) {
            fprintf(stderr, "[ERROR] Could not open file: %s\n", path);
            fflush(stderr); // Forces the terminal to display the message right away
            return;
        }

        size_t bytes = fread(shader_buffer, 1, shader_buffer_length - 1, stream);
        shader_buffer[bytes] = '\0';

        fclose(stream);
    }

};

};
