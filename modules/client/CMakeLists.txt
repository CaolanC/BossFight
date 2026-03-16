file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(ClientLib ${CLIENT_SOURCES}
        src/NetClient.cpp
        src/NetClient.hpp
        src/WebsocketMsg.cpp
        src/WebsocketMsg.hpp)
target_include_directories(ClientLib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(ClientLib
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
)