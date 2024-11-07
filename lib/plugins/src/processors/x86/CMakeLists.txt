cmake_minimum_required(VERSION 3.25)

project(x86)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE
        x86_common.cpp
        x86_lifter.cpp
        x86.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Zydis
        redasm::lib
)

setup_compiler(${PROJECT_NAME})
