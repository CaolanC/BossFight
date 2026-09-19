file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(PakLib ${CLIENT_SOURCES})

target_include_directories(PakLib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(PakLib
    PUBLIC
		ClientLib
		crossguid
)

target_compile_definitions(PakLib PRIVATE 
    SLJA_ASSETS_DIR="${ASSETS_DIR}"
    SLJA_SAVES_DIR="${SAVES_DIR}"
)
