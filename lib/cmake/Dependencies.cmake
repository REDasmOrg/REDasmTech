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

    CPMAddPackage(
        NAME RapidJSON
        GIT_TAG "24b5e7a8b27f42fa16b96fc70aade9106cf7102f"
        GITHUB_REPOSITORY "Tencent/rapidjson"
        OPTIONS
            "RAPIDJSON_BUILD_TESTS OFF"
            "RAPIDJSON_BUILD_DOC OFF"
            "RAPIDJSON_BUILD_EXAMPLES OFF"
            "RAPIDJSON_BUILD_CXX11 OFF"
            "RAPIDJSON_BUILD_CXX17 ON"
    )

    if(RapidJSON_ADDED)
        add_library(rapidjson INTERFACE IMPORTED)
        target_include_directories(rapidjson INTERFACE "${RapidJSON_SOURCE_DIR}/include")

        target_compile_definitions(rapidjson
            INTERFACE
                "RAPIDJSON_HAS_STDSTRING"
        )
    endif()
endfunction()
