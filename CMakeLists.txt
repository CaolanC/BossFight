cmake_minimum_required(VERSION 3.28.3)
project(Soulja LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL TRUE)

# Paths
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/res/assets/")
set(SAVES_DIR "${CMAKE_SOURCE_DIR}/res/saves/")

# ---------------------------------------------------------
# Find Dependencies via Conan / System
# ---------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)            # Provided by Conan
find_package(EnTT REQUIRED)           # Provided by Conan
find_package(concurrentqueue REQUIRED)# Provided by Conan
find_package(cglm REQUIRED)           # Provided by Conan

find_package(SDL3 REQUIRED)

# 1. If Conan created `sdl::sdl`, promote it to GLOBAL so subdirectories see it

set_target_properties(sdl::sdl PROPERTIES IMPORTED_GLOBAL TRUE)

include_directories(SYSTEM ${CONAN_INCLUDE_DIRS})

if(TARGET glm::glm)
    set_target_properties(glm::glm PROPERTIES IMPORTED_GLOBAL TRUE)
    if(DEFINED GLM_INCLUDE_DIRS)
        target_include_directories(glm::glm INTERFACE ${GLM_INCLUDE_DIRS})
    elseif(DEFINED glm_INCLUDE_DIRS)
        target_include_directories(glm::glm INTERFACE ${glm_INCLUDE_DIRS})
    elseif(DEFINED GLM_INCLUDE_DIR)
        target_include_directories(glm::glm INTERFACE ${GLM_INCLUDE_DIR})
    endif()
endif()

if(DEFINED SDL3_INCLUDE_DIRS)
    target_include_directories(sdl::sdl INTERFACE ${SDL3_INCLUDE_DIRS})
elseif(DEFINED sdl_INCLUDE_DIRS)
    target_include_directories(sdl::sdl INTERFACE ${sdl_INCLUDE_DIRS})
elseif(DEFINED SDL3_INCLUDE_DIR)
    target_include_directories(sdl::sdl INTERFACE ${SDL3_INCLUDE_DIR})
endif()

set(SDL_TARGET sdl::sdl)

# Global Linker Interception
if(TARGET concurrentqueue::concurrentqueue AND NOT TARGET concurrentqueue)
    add_library(concurrentqueue INTERFACE IMPORTED GLOBAL)
    target_link_libraries(concurrentqueue INTERFACE concurrentqueue::concurrentqueue)
endif()

# ---------------------------------------------------------
# Subdirectories (Third Party)
# ---------------------------------------------------------
add_subdirectory(thirdpartypoopers/libhv)
add_subdirectory(thirdpartypoopers/glad)
add_subdirectory(thirdpartypoopers/FastNoiseLite)
add_subdirectory(thirdpartypoopers/crossguid)
add_subdirectory(thirdpartypoopers/tinygltf)
add_subdirectory(thirdpartypoopers/PolyVox)
add_subdirectory(thirdpartypoopers/header_only)
add_subdirectory(imguicmakeholder/imgui)

# --- Fix 2: Internal Application Modules ---
# SharedLib MUST be processed before ClientLib, which happens naturally inside add_subdirectory
add_subdirectory(src/modules/shared)
add_subdirectory(src/modules/server)
add_subdirectory(src/modules/client)
add_subdirectory(src/modules/gui)
add_subdirectory(src/modules/editor)

# Testing
enable_testing()
find_package(GTest REQUIRED)
add_subdirectory(src/tests)

set_target_properties(hv PROPERTIES
        IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/libhv.dll.a"
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/bin/libhv.dll"
)

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
    imgui
    ${SDL_TARGET} 
    cglm::cglm 
    OpenGL::GL
)
set_target_properties(GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Post-Build Steps
if (WIN32)
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
