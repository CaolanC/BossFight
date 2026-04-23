#pragma once

#include <entt/entt.hpp>
#include <component/Core.hpp>

namespace animation {
    static void spin(entt::registry& r) {
        auto v = r.view<component::transform, component::spin>();
        for (auto [e, tr, a]: v.each()) {

        }
    }
}