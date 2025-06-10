cmake_minimum_required(VERSION 3.25)

project(elf)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE
        elf.cpp
        elf_types.cpp
        elf_dwarf.cpp
        elf_format.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        redasm::lib
)

setup_compiler(${PROJECT_NAME})
