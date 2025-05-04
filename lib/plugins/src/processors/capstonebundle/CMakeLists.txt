cmake_minimum_required(VERSION 3.25)

project(capstonebundle)

include("../../../cmake/CPM.cmake")

CPMAddPackage(
    NAME capstone
    GIT_TAG "5.0.3"
    GITHUB_REPOSITORY "capstone-engine/capstone"
    OPTIONS 
        "CAPSTONE_ARCHITECTURE_DEFAULT OFF"
        "CAPSTONE_ARM_SUPPORT ON"
        "CAPSTONE_ARM64_SUPPORT ON"
)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE
        plugin.cpp
        capstone.cpp
        arm/arm32.cpp
        arm/arm32_macros.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        redasm::lib
        capstone
)

setup_compiler(${PROJECT_NAME})
