#pragma once

#include <crossguid/guid.hpp>

#include <optional>

#include <rendering/CPUTexture.hpp>
#include <rendering/GPUTexture.hpp>

using TextureAssetHandle = xg::Guid;

namespace rendering {

struct TextureAsset {
	std::optional<CPUTexture> cpu_texture;
	std::optional<GPUTexture> gpu_texture;
	TextureAssetHandle handle;
};

};
