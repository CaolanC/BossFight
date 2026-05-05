file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(EditorLib ${CLIENT_SOURCES}
        src/EditorPanels.cpp
        src/EditorPanels.hpp
        src/EditorApp.hpp
)
target_include_directories(EditorLib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(EditorLib
        PUBLIC
        tinygltf
        glad
        EnTT::EnTT
        SDL3::SDL3
        glm::glm
        par_shapes
        crossguid
        hv
        concurrentqueue
        SharedLib
        nlohmann_json
        imgui
        ClientLib
)