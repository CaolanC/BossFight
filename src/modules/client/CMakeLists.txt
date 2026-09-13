file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(ClientLib ${CLIENT_SOURCES})

target_include_directories(ClientLib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(ClientLib
    PUBLIC
    	sdl::sdl
        tinygltf
        glad
        EnTT::EnTT
        glm::glm
        par_shapes
        crossguid
        hv
        concurrentqueue::concurrentqueue
        SharedLib
        nlohmann_json
)

target_compile_definitions(ClientLib PRIVATE 
    SLJA_ASSETS_DIR="${ASSETS_DIR}"
    SLJA_SAVES_DIR="${SAVES_DIR}"
)
