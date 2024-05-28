include(cmake/CPM.cmake)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

function(setup_dependencies)
endfunction()
