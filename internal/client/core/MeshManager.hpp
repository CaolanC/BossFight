#pragma once

#include <core/Mesh.hpp>
#include <core/defines.hpp>
#include <map>
#include <crossguid/guid.hpp>

namespace core {

class MeshManager
{
public:
    xg::Guid createIndexedMeshFromVertices(std::vector<float> vertices, std::vector<unsigned int> indices, core::MeshSerialiser serialiser) {
        core::Mesh mesh(vertices, indices, serialiser);
        auto id = xg::newGuid();
        mesh_map.insert({id, mesh});
        return id;
    };

    xg::Guid add_mesh(core::Mesh mesh) {
        auto g = xg::newGuid();
        mesh_map.insert({g, mesh});

        return g;
    }

    core::Mesh get_mesh(MeshHandle id) {
        return mesh_map.at(id);
    }

private:
    std::map<xg::Guid, core::Mesh> mesh_map;

};

}