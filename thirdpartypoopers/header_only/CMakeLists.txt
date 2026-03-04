cmake_minimum_required(VERSION 3.28.3)
project(par_shapes)


add_library(par_shapes STATIC
        par_shapes/par_shapes.cpp
)
target_include_directories(par_shapes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/par_shapes)
