#include <Runtime.hpp>
#include <systems/Transform.hpp>
#include <systems/Debug.hpp>
#include <systems/UserControl.hpp>

#include <glad/glad.h>
#include <SDL3/SDL.h>

namespace runtime {

Runtime::Runtime() {
	systems.push_back(systems::UserControl);
	systems.push_back(systems::Transform);
	systems.push_back(systems::Debug);
};

void Runtime::update() {
	for(auto& system : systems) {
		system(active_scene);
	}
};

entt::entity Runtime::create_entity() {
	return active_scene.create();
};

void Runtime::create_scene(const std::string scene_name) {
	scene_manager.create_scene(scene_name);
};

entt::registry& Runtime::get_scene_registry() {
	return active_scene;
};

const std::vector<std::string> Runtime::get_scene_names() {
	return scene_manager.get_scene_names();
};

void Runtime::init() {
	resource_manager.init();
	renderer.init();
};

unsigned int Runtime::render_texture(int w, int h) {
	renderer.render_to_texture(w, h, active_scene);
    return renderer.get_color_texture();
}

void Runtime::render_to_window(int w, int h) {
	renderer.render_to_window(w, h, active_scene);
};

void Runtime::begin_input_frame() {
	//entt::registry& r = active_scene;
	auto& ms = active_scene.ctx().get<component::mouse_state>();

	ms.dx = 0.0f;
	ms.dy = 0.0f;
}


void Runtime::process_input_event(const SDL_Event& event) {
    entt::registry& r = active_scene;
    auto& kb = r.ctx().get<component::keyboard_state>();
    auto& ms = r.ctx().get<component::mouse_state>();

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            if (event.key.scancode < SDL_SCANCODE_COUNT) {
                kb.down[event.key.scancode] = true;
            }
            break;

        case SDL_EVENT_KEY_UP:
            if (event.key.scancode < SDL_SCANCODE_COUNT) {
                kb.down[event.key.scancode] = false;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION: {
            float rel_x = event.motion.xrel;
            float rel_y = event.motion.yrel;

            // Fallback for Linux/VirtualBox
            if (rel_x == 0.0f && rel_y == 0.0f) {
                rel_x = event.motion.x - ms.x;
                rel_y = event.motion.y - ms.y;
            }

            ms.x = event.motion.x;
            ms.y = event.motion.y;
            ms.dx += rel_x;
            ms.dy += rel_y;
            break;
        }
    
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button < ms.down.size()) {
                ms.down[event.button.button] = true;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button < ms.down.size()) {
                ms.down[event.button.button] = false;
            }
            break;

        default:
            break;
    }
}

};
