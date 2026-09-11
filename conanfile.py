from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout

class SouljaConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    requires = (
        "sdl/3.4.8",
        "glm/1.0.1",
        "entt/3.14.0",
        "cglm/0.9.6",
        "gtest/1.15.0",
        "concurrentqueue/1.0.4",
    )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        # Guarantees headers are attached transitively to generated CMake targets
        for req in self.requires.values():
            deps.set_property(req.ref.name, "transitive_headers", True)
        deps.generate()
