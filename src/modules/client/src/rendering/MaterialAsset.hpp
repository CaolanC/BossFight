#pragma once

#include <crossguid/guid.hpp>

#include <rendering/TextureAsset.hpp>
#include <core/defines.hpp>

namespace rendering {
    using MaterialAssetHandle = xg::Guid;

    struct MaterialAsset {
	MaterialAssetHandle handle;
	TextureAsset texture_asset; // Should absolutely be a handle
	ShaderProgramHandle shader_program_handle;
	// texcord,
	// texture(s?) handle
    };

};
