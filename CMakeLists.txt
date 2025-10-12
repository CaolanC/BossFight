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
add_subdirectory(thirdpartypoopers/PolyVox)

set(CLIENT_SHARED_LIBS glm::glm glad SDL3::SDL3 OpenGL::GL m fastnoiselitelib EnTT::EnTT hv crossguid tinygltf PolyVoxCore PolyVoxUtil)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SERVER_SRC_FILES "${CMAKE_SOURCE_DIR}/src/server/*.cpp")
add_library(ServerLib ${SERVER_SRC_FILES})

target_include_directories(ServerLib PUBLIC ${CMAKE_SOURCE_DIR}/internal)

target_link_libraries(ServerLib
  PUBLIC EnTT::EnTT
  PRIVATE concurrentqueue hv crossguid
)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC
        SDL3::SDL3
)

add_executable(Client apps/client/main.cpp)
file(GLOB_RECURSE CLIENT_SRC_FILES "${CMAKE_SOURCE_DIR}/src/client/*.cpp")
add_library(ClientLib ${CLIENT_SRC_FILES})
target_include_directories(ClientLib PUBLIC
    ${CMAKE_SOURCE_DIR}/internal/client
)

target_link_libraries(ClientLib imgui glm::glm ${CLIENT_SHARED_LIBS})

set_target_properties(Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/internal/client)
target_link_libraries(Client PUBLIC imgui ClientLib ${CLIENT_SHARED_LIBS})
target_compile_definitions(Client PRIVATE SLJA_WINDOW_TITLE="Soulja")
target_compile_definitions(ClientLib PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")

add_executable(Server apps/server/main.cpp)
set_target_properties(Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries(Server PRIVATE ServerLib concurrentqueue EnTT::EnTT hv crossguid)
target_compile_definitions(Server PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")

target_include_directories(Client
    PRIVATE
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/stb
)

add_executable(GUI apps/gui/main.cpp)
target_include_directories(GUI PRIVATE thirdpartypoopers/libhv)
target_link_libraries(GUI PRIVATE hv imgui SDL3::SDL3 OpenGL::GL)

set_target_properties(GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")