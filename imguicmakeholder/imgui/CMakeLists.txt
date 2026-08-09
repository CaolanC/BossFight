add_library(imgui STATIC
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/imgui_demo.cpp
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/backends/imgui_impl_sdl3.cpp
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui
        ${CMAKE_SOURCE_DIR}/thirdpartypoopers/imgui/backends
)

target_link_libraries(imgui PUBLIC
        SDL3::SDL3
)

find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC OpenGL::GL)