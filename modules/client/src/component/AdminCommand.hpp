#pragma once

namespace admin {
    using CommandType =  enum {
        CreateMesh,
        CreateMaterial,
        AttachMesh,
        AttachMaterial
    };
}

namespace component {
    struct AdminCommand {};
}