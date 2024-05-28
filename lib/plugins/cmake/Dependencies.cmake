include(cmake/CPM.cmake)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

function(setup_dependencies)
    CPMAddPackage(
        NAME zydis
        VERSION "4.1.0"
        GITHUB_REPOSITORY "zyantific/zydis"

        OPTIONS 
            "ZYDIS_BUILD_TOOLS OFF"
            "ZYDIS_BUILD_EXAMPLES OFF"
    )
endfunction()
