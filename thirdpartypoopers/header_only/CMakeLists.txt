cmake_minimum_required(VERSION 3.28.3)
project(par_shapes)


add_library(par_shapes INTERFACE)
target_include_directories(par_shapes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/par_shapes)
