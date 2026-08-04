cmake_minimum_required(VERSION 3.28.3)
project(Soulja LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

# Paths
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/res/assets/")
set(SAVES_DIR "${CMAKE_SOURCE_DIR}/res/saves/")

# ---------------------------------------------------------
# Find Dependencies via Conan / System
# ---------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)          # Provided by Conan (sdl/3.x maps to SDL3)
find_package(glm REQUIRED)           # Provided by Conan
find_package(EnTT REQUIRED)          # Provided by Conan
find_package(concurrentqueue REQUIRED) # Provided by Conan
find_package(cglm REQUIRED)          # Provided by Conan

# Set fallback or standard target variable 
set(SDL_TARGET SDL3::SDL3-static)
if(NOT TARGET ${SDL_TARGET})
    set(SDL_TARGET SDL3::SDL3)
endif()

# ---------------------------------------------------------
# Global Linker Interception (Fixes raw -lconcurrentqueue leaks)
# ---------------------------------------------------------
# If a subdirectory requests 'concurrentqueue' instead of the namespaced 
# 'concurrentqueue::concurrentqueue', this redirect block catches it 
# and prevents the linker from searching for a non-existent static binary.
if(TARGET concurrentqueue::concurrentqueue AND NOT TARGET concurrentqueue)
    add_library(concurrentqueue INTERFACE IMPORTED GLOBAL)
    target_link_libraries(concurrentqueue INTERFACE concurrentqueue::concurrentqueue)
endif()

# ---------------------------------------------------------
# Subdirectories (Custom/In-Tree Third Party & App Modules)
# ---------------------------------------------------------
# Kept in-tree because they are highly customized or absent from central repos
add_subdirectory(thirdpartypoopers/libhv)
add_subdirectory(thirdpartypoopers/glad)
add_subdirectory(thirdpartypoopers/FastNoiseLite)
add_subdirectory(thirdpartypoopers/crossguid)
add_subdirectory(thirdpartypoopers/tinygltf)
add_subdirectory(thirdpartypoopers/PolyVox)
add_subdirectory(thirdpartypoopers/header_only)
add_subdirectory(imguicmakeholder/imgui)

# Internal Application Modules
add_subdirectory(src/modules/server)
add_subdirectory(src/modules/shared)
add_subdirectory(src/modules/client)
add_subdirectory(src/modules/gui)
add_subdirectory(src/modules/editor)

# Testing
enable_testing()
find_package(GTest REQUIRED) # Provided by Conan
add_subdirectory(src/tests)

# Manual overrides for custom built libhv binary fallbacks if needed
set_target_properties(hv PROPERTIES
        IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/libhv.dll.a"
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/bin/libhv.dll"
)

set(EXTRA_M_LIB)
if(UNIX AND NOT APPLE)
    set(EXTRA_M_LIB m)
endif()

# ---------------------------------------------------------
# Module-Level Linkage (Fixes internal library module headers)
# ---------------------------------------------------------
# Explicitly feeds Conan dependencies down into your individual library modules
# before they are compiled by their respective add_subdirectories.
if(TARGET ServerLib)
    target_link_libraries(ServerLib PRIVATE concurrentqueue::concurrentqueue EnTT::EnTT glm::glm)
endif()

if(TARGET ClientLib)
    target_link_libraries(ClientLib PRIVATE EnTT::EnTT glm::glm ${SDL_TARGET})
endif()


# ---------------------------------------------------------
# Executables
# ---------------------------------------------------------
# Server App
add_executable(Server src/apps/server/main.cpp)
set_target_properties(Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries(Server PRIVATE 
    ServerLib 
    SharedLib 
    concurrentqueue::concurrentqueue 
    EnTT::EnTT 
    hv 
    crossguid 
    glm::glm
)
target_compile_definitions(Server PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")

# GUI Client App
add_executable(GUI src/apps/gui/main.cpp)
target_link_libraries(GUI PUBLIC 
    GuiLib 
    glad 
    EditorLib 
    ClientLib 
    ${SDL_TARGET} 
    cglm::cglm 
    OpenGL::GL
)
set_target_properties(GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_compile_definitions(ClientLib PRIVATE SLJA_ASSETS_DIR="${ASSETS_DIR}")
target_compile_definitions(ClientLib PRIVATE SLJA_SAVES_DIR="${SAVES_DIR}")

# ---------------------------------------------------------
# Post-Build Steps
# ---------------------------------------------------------
if (WIN32)
    # Automatically copies runtime dependencies (like libhv if built shared)
    add_custom_command(TARGET Server POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:Server>
            $<TARGET_FILE_DIR:Server>
            COMMAND_EXPAND_LISTS
    )
    add_custom_command(TARGET GUI POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bin/libhv.dll"
            $<TARGET_FILE_DIR:GUI>
    )
endif()

