#pragma once

#include <optional>

#include <crossguid/guid.hpp>

namespace rendering {

using MeshAssetHandle = xg::Guid;

struct MeshAsset {
    std::optional<GPUMesh> gpu_mesh;
    std::optional<CPUMesh> cpu_mesh;
    xg::Guid id; // Unsure whether to keep this or if the map stores this identifier.
};

};
