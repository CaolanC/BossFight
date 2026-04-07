file(GLOB_RECURSE SHARED_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_library(SharedLib ${SHARED_SOURCES})
target_include_directories(SharedLib
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(SharedLib
    PUBLIC
        glm::glm
        nlohmann_json
        crossguid
)