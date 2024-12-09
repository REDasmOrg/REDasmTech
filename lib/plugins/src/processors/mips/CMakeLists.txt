cmake_minimum_required(VERSION 3.25)

project(mips)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE
        mips.cpp
        mips_macrodecoder.cpp
        mips_decoder.cpp
        mips_format.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        redasm::lib
)

setup_compiler(${PROJECT_NAME})
