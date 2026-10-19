#pragma once

#include <crossguid/guid.hpp>

#include <optional>
#include <vector>

#include <rendering/CPUTexture.hpp>
#include <rendering/GPUTexture.hpp>

using TextureAssetHandle = xg::Guid;

namespace rendering {

enum class TextureDimension {
	Texture2D,
	Texture2DArray,
	TextureCube,
	Texture3D
};

struct TextureAsset {
	std::vector<CPUTexture> cpu_textures;
	std::optional<GPUTexture> gpu_texture;
	TextureAssetHandle handle;
	TextureDimension dimension;
};

};
