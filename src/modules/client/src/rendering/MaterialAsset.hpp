#pragma once

#include <crossguid/guid.hpp>

#include <rendering/TextureAsset.hpp>
#include <core/defines.hpp>
#include <rendering/ShaderProgramAsset.hpp>

namespace rendering {
    using MaterialAssetHandle = xg::Guid;

    struct MaterialAsset {
		MaterialAssetHandle handle;
		std::optional<TextureAssetHandle> base_color_texture_handle;
		ShaderProgramAssetHandle shader_program_handle;
		//ShaderProgramHandle shader_program_handle;
    };

};
