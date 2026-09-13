#pragma once

#include <rendering/MeshAsset.hpp>

#include <map>

class NewMeshManager {
  public:
    NewMeshManager();
  private:
    std::map<MeshAssetHandle, MeshAsset> mesh_assets;

}
