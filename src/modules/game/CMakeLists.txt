file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(GameLib ${CLIENT_SOURCES})

target_include_directories(GameLib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(GameLib
    PUBLIC
    	sdl::sdl
        glad
        EnTT::EnTT
        crossguid
		ClientLib
)

target_compile_definitions(GameLib PRIVATE 
    SLJA_ASSETS_DIR="${ASSETS_DIR}"
    SLJA_SAVES_DIR="${SAVES_DIR}"
)
