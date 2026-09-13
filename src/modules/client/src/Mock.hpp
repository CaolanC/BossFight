#pragma once

#ifdef MOCK

#include bsft

auto& bsft = bsft();
auto& scene = bsft.get_scene("Default Scene"); // Pure state with configurable resource components
scene.add_ent_from_mt() 

auto& static_modelsA

auto& resource_manager = bsft::resource_manager(); // <- Okay so this is independent of scenes entirely. It lives in the background. But
	// we configure it with components 


// So bsft is potentially a state machine? Minimum one scene, makes sense.
// The hard thing is how do we model scripting in the editor to make a new scene
// Okay so a scene is basically a registry, but we want a root entity.
// The client has an active scene/registry.
//
// Scripting in the editor is systems. We shouldn't be able to create a new scene from a system. I don't think.
// Systems take in a registry reference.
//
// So we have this idea of a runtime. It orders the systems, this needs to be dynamic. It holds the scenes. It holds the gpu/cpu resources. The client
// HAS a runtime? That way the client handles the window management and the user input handoff. 
//
// So:
//
// Runtime:
// - Map of strings to scenes
// - List of systems, but we need to account for multithreading
// - Shouldn't manage any resources, just configure and run the simulation.
// - An
// - Needs to be able to flag which resources need to be loaded. Unconditional. Conditional. A component. Default is unconditional. ConditionalLoading component.
// - How do we represent resources to the runtime if it shouldn't really manage them?
//
// ResourceManager:
//
// Renderer: != RenderSystem
// - The RenderSystem has an instance of the Renderer. The renderer exposes an API that the RenderSystem can use to pass information and draw things.

// Execution Cycle
//

#endif
