#pragma once

#include <crossguid/guid.hpp>

#include <rendering/TextureAsset.hpp>

namespace rendering {
    using MaterialAssetHandle = xg::Guid;

    struct MaterialAsset {
	MaterialAssetHandle handle;
	TextureAsset texture_asset;
	// texcord,
	// texture(s?) handle
    };

};
