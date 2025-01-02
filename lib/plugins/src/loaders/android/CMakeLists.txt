cmake_minimum_required(VERSION 3.25)

project(dex)

add_library(${PROJECT_NAME} SHARED)
add_subdirectory(libdex)

target_sources(${PROJECT_NAME} 
    PRIVATE
        android.cpp
        demangler.cpp
        dex.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        redasm::lib
        libdex
)

setup_compiler(${PROJECT_NAME})
