#include <Runtime.hpp>
#include <systems/Transform.hpp>
#include <systems/Debug.hpp>

#include <glad/glad.h>

namespace runtime {

Runtime::Runtime() {
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

void Runtime::init_embedded() {
    resource_manager.init();
    renderer.init_ubos();

};

void Runtime::ensure_framebuffer(int w, int h) { // This needs moved to the runtime api via the editor STAT. like we need that shyt GONE.
    if (framebuffer != 0 && framebuffer_width == w && framebuffer_height == h) {
        return;
    }

    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &color_texture);
        glDeleteRenderbuffers(1, &depth_rbo);
    }

    framebuffer_width = w;
    framebuffer_height = h;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::printf("framebuffer incomplete\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void Runtime::render_to_texture(int w, int h) {
    init_embedded();
    ensure_framebuffer(w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer.new_render(active_scene, w, h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Runtime::get_render_texture() const {
    return color_texture;
}



};
