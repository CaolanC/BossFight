file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(GuiLib ${GUI_SOURCES}
        src/Empty.cpp
        src/Empty.hpp)
target_include_directories(GuiLib
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(GuiLib
        PUBLIC
        imgui
        hv
)