#pragma once

#include <core/MeshManager.hpp>

namespace core
{
    class ObjectFactory {
        public:
        ObjectFactory(core::MeshManager _mesh_manager) {
            mesh_manager = _mesh_manager;
        }

        

        private:

        core::MeshManager mesh_manager;
    };
}