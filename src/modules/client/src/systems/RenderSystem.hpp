#pragma once

#include <entt/entt.hpp>

namespace systems {

#ifdef RENDERSYSTEM

class RenderSystem {

	void run(entt::registry& reg);

};

#endif

};
