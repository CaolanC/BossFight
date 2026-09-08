#pragma once

#include <crossguid/guid.hpp>

#include <optional>

#include <rendering/CPUTexture.hpp>

using TextureAssetHandle = xg::Guid;

namespace rendering {

struct TextureAsset {
	std::optional<CPUTexture> cpu_texture;
};

};
