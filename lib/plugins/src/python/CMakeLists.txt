cmake_minimum_required(VERSION 3.25)

project(rdpython)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Python3 COMPONENTS Development)

include(cmake/Dependencies.cmake)
setup_dependencies()

add_library(${PROJECT_NAME} SHARED)

include(cmake/Compiler.cmake)
setup_compiler(${PROJECT_NAME})

target_sources(${PROJECT_NAME} 
    PRIVATE
        src/utils.cpp
        src/byteorder.cpp
        src/rdil.cpp
        src/buffer.cpp
        src/stream.cpp
        src/common.cpp
        src/plugin.cpp
        src/typing.cpp
        src/analyzer.cpp
        src/loader.cpp
        src/redasm.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Python3::Python
        redasm::lib
)
