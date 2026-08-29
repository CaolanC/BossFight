#pragma once

#include <crossguid/guid.hpp>

namespace rendering {
    using MaterialAssetHandle = xg::Guid;

    struct MaterialAsset {
	MaterialAssetHandle handle;
    };

};
