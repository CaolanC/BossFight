cmake_minimum_required(VERSION 3.28.3)
project(Soulja LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)


if(WIN32)

    set(SDL_VIDEO ON CACHE BOOL "" FORCE)
else() # Linux/BSD
    set(SDL_VIDEO            ON  CACHE BOOL "" FORCE)
    set(SDL_VIDEO_X11        ON  CACHE BOOL "" FORCE)
    set(SDL_VIDEO_WAYLAND    ON  CACHE BOOL "" FORCE)
    set(SDL_VIDEO_OPENGL     ON  CACHE BOOL "" FORCE)
    set(SDL_VIDEO_OPENGL_GLX ON  CACHE BOOL "" FORCE)
    set(SDL_VIDEO_OPENGL_EGL ON  CACHE BOOL "" FORCE)
endif()


set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)

set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)


set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/")
find_package(OpenGL REQUIRED)

set(SDL_TARGET SDL3::SDL3-static)

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
add_subdirectory(thirdpartypoopers/header_only)
add_subdirectory(thirdpartypoopers/imgui)

add_subdirectory(modules/server)
add_subdirectory(modules/shared)
add_subdirectory(modules/client)
add_subdirectory(modules/gui)

if(NOT TARGET ${SDL_TARGET})
    if(TARGET SDL3::SDL3)
        set(SDL_TARGET SDL3::SDL3)
    else()
        message(FATAL_ERROR
                "SDL3 targets not defined. Expected SDL3::SDL3-static or SDL3::SDL3. "
                "Check the submodule path and SDL_* options.")
    endif()
endif()

set(EXTRA_M_LIB)
if(UNIX AND NOT APPLE)
    set(EXTRA_M_LIB m)
endif()

add_executable(Server apps/server/main.cpp)
set_target_properties(Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_link_libraries(Server PRIVATE ServerLib SharedLib concurrentqueue EnTT::EnTT hv crossguid glm::glm)
target_compile_definitions(Server PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")

add_executable(Client apps/client/main.cpp)

set_target_properties(Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_link_libraries(Client PUBLIC imgui ClientLib ${CLIENT_SHARED_LIBS})
target_compile_definitions(Client PRIVATE SLJA_WINDOW_TITLE="Soulja")
target_compile_definitions(ClientLib PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")

add_executable(GUI apps/gui/main.cpp)
target_link_libraries(GUI PRIVATE GuiLib)