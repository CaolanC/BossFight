cmake_minimum_required(VERSION 3.28.3)
project(Soulja LANGUAGES C CXX)

# SDL & OpenGL switches ---
set(SDL_VIDEO            ON  CACHE BOOL "" FORCE)
set(SDL_VIDEO_X11        ON  CACHE BOOL "" FORCE)
set(SDL_VIDEO_WAYLAND    ON  CACHE BOOL "" FORCE)
set(SDL_VIDEO_OPENGL     ON  CACHE BOOL "" FORCE)
set(SDL_VIDEO_OPENGL_GLX ON  CACHE BOOL "" FORCE)
set(SDL_VIDEO_OPENGL_EGL ON  CACHE BOOL "" FORCE)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/")
find_package(OpenGL REQUIRED)

add_subdirectory(thirdpartypoopers/libhv)
add_subdirectory(thirdpartypoopers/libsdl-org)
add_subdirectory(thirdpartypoopers/cglm)
add_subdirectory(thirdpartypoopers/glad)
add_subdirectory(thirdpartypoopers/FastNoiseLite)
add_subdirectory(thirdpartypoopers/glm)
add_subdirectory(thirdpartypoopers/entt)
add_subdirectory(thirdpartypoopers/concurrentqueue)
add_subdirectory(thirdpartypoopers/crossguid)
add_subdirectory(thirdpartypoopers/tinygltf)



set(CLIENT_SHARED_LIBS cglm glad SDL3::SDL3 OpenGL::GL m fastnoiselitelib SouljaSharedLib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SERVER_SRC_FILES "${CMAKE_SOURCE_DIR}/src/server/*.cpp")
add_library(ServerLib ${SERVER_SRC_FILES})

target_include_directories(ServerLib PUBLIC ${CMAKE_SOURCE_DIR}/internal)

target_link_libraries(ServerLib
  PUBLIC EnTT::EnTT
  PRIVATE concurrentqueue hv crossguid
)

add_executable(Client apps/client/main.cpp
        internal/client/component/AdminCommand.hpp
        internal/client/systems/MeshLoading.hpp
        internal/client/systems/Init.hpp
        internal/client/utils/Model.hpp
        internal/client/core/ModelManager.hpp)

set_target_properties(Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/internal/client)
target_link_libraries(Client PRIVATE EnTT::EnTT glad SDL3::SDL3 OpenGL::GL hv glm::glm crossguid tinygltf)
target_compile_definitions(Client PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}" SLJA_WINDOW_TITLE="Soulja")

add_executable(Server apps/server/main.cpp)
set_target_properties(Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries(Server PRIVATE ServerLib concurrentqueue EnTT::EnTT hv crossguid)
target_compile_definitions(Server PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")
