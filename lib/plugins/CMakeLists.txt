cmake_minimum_required(VERSION 3.25)

project(plugins)

include(cmake/Dependencies.cmake)
setup_dependencies()

include(cmake/Compiler.cmake)

add_subdirectory(src/processors/x86)
add_subdirectory(src/processors/mips)
