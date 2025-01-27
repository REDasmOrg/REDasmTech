include(cmake/CPM.cmake)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

function(setup_dependencies)
    CPMAddPackage("gh:Dax89/QHexView#40f40f5d6a546646904c094ebefc8216067ab586")
endfunction()
