#pragma once

#include <entt/entt.hpp>
#include <rendering/ResourceManager.hpp>
#include <rendering/EntityFactory.hpp>
#include <SceneManager.hpp>
#include <Renderer.hpp>

#include <SDL3/SDL.h>

#include <vector>

namespace runtime {

class Runtime {
	public:
	Runtime();
	entt::entity create_entity();
	entt::registry& get_scene_registry();
	const std::vector<std::string> get_scene_names();
	void create_scene(const std::string);
	rendering::EntityFactory entity_factory; // Only public for now, this stuff should happen internally soon.	
	SceneManager scene_manager; // This too.
	rendering::ResourceManager resource_manager = rendering::ResourceManager(); // This too.
	entt::registry& active_scene = scene_manager.get_scene("Default Scene"); // This too.

	//void ensure_framebuffer(int w, int h); // Ideally these don't exist here at all, but this too lol.
    unsigned int render_texture(int w, int h);
    //unsigned int get_render_texture() const;

	void process_input_event(const SDL_Event& event);
	void begin_input_frame();
	void render_to_window(int w, int h);
	


	void init();
	void init_embedded(); // This too but maybe an init function instead? dunno yet :DDDD!!!!. ;)
	void update(); // This too.
	private:
	client::Renderer renderer = client::Renderer(resource_manager);
	std::vector<void (*)(entt::registry&)> systems;
    //unsigned int framebuffer = 0; // I really don't want this stuff in here, so we can probabaly move it to the renderer, and expose an api instead.
    //unsigned int color_texture = 0;
    //unsigned int depth_rbo = 0;
    //int framebuffer_width = 0;
    //int framebuffer_height = 0;

};

#ifdef RUNTIME_EDITOR_API


// Scene API
switch_scene();
create_scene();
delete_scene();
run_scene();
// Maybe potentially snapshot scene we need to think about how we're handling running the runtime in the editor.

// Entity API
create_entity();
delete_entity();
add_component(e, component);


// Editor Calls for Functionality

get_entities(); // For our entity list; We potentially just want to hand back a const reference to the registry so we can do our component things too.
// Need to account for if the scene switches then.
get_scenes(); // For our scene list and switcher



#endif

};
