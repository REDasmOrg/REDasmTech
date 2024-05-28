cmake_minimum_required(VERSION 3.25)

set(PARENT_TARGET "${PROJECT_NAME_LIB}")

project(CmakeConfigPackageTests LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
    find_package(${PARENT_TARGET} CONFIG REQUIRED)

    if(NOT TARGET ${PARENT_TARGET}_options)
        message(FATAL_ERROR "Required config package not found!")
        return()
    endif()
endif()

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
add_executable(tests)
setup_compiler(tests)

target_sources(tests
    PRIVATE
        main.cpp
)

target_link_libraries(tests
    PRIVATE
        Catch2::Catch2
        ${PARENT_TARGET}
)

# catch_discover_tests(tests
#     TEST_PREFIX "unittests."
#     REPORTER XML
#     OUTPUT_DIR .
#     OUTPUT_PREFIX "unittests."
#     OUTPUT_SUFFIX .xml
# )
