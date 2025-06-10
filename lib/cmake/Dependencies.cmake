include(cmake/CPM.cmake)
include(cmake/CPM_SQLite3.cmake)

find_package(Threads REQUIRED)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

function(setup_dependencies)
    # Priority over old Catch2's versions from dependencies
    if(BUILD_TESTING)
        CPMAddPackage("gh:catchorg/Catch2@3.5.0")
    endif()

    CPMAddPackage("gh:TartanLlama/optional@1.1.0")
    CPMAddPackage("gh:fmtlib/fmt#10.2.1")
    CPMAddPackage("gh:richgel999/miniz#c883286f1a6443720e7705450f59e579a4bbb8e2")

    CPMAddPackage(
        NAME spdlog
        VERSION "1.14.1"
        GITHUB_REPOSITORY "gabime/spdlog"
        OPTIONS "SPDLOG_FMT_EXTERNAL ON"
    )
endfunction()
