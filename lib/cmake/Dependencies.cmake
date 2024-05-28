include(cmake/CPM.cmake)

find_package(Threads REQUIRED)
find_package(Python3 COMPONENTS Development)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

function(setup_dependencies)
    # Priority over old Catch2's versions from dependencies
    if(BUILD_TESTING)
        CPMAddPackage("gh:catchorg/Catch2@3.5.0")
    endif()

    CPMAddPackage("gh:TartanLlama/expected@1.1.0")
    CPMAddPackage("gh:TartanLlama/optional@1.1.0")
    CPMAddPackage("gh:fmtlib/fmt#10.1.1")

    CPMAddPackage(
        NAME spdlog
        VERSION "1.12.0"
        GITHUB_REPOSITORY "gabime/spdlog"
        OPTIONS "SPDLOG_FMT_EXTERNAL ON"
    )
endfunction()
