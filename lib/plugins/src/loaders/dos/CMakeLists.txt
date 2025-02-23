cmake_minimum_required(VERSION 3.25)

project(dos)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE
        plugin.cpp
        com.cpp
        dos.cpp
        env.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        redasm::lib
)

setup_compiler(${PROJECT_NAME})
