cmake_minimum_required(VERSION 3.25)

project(redasm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(EXISTS src/main.cpp)
    set(PROJECT_NAME_LIB "${PROJECT_NAME}_lib")
else()
    set(PROJECT_NAME_LIB "${PROJECT_NAME}")
endif()

include(cmake/Dependencies.cmake)
setup_dependencies()

add_library(${PROJECT_NAME_LIB} SHARED)

target_compile_definitions(${PROJECT_NAME_LIB}
    PUBLIC
        MINIZ_NO_ZLIB_COMPATIBLE_NAMES

    PRIVATE
      SHARED_OBJECT_EXT="${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

include(cmake/Compiler.cmake)
setup_compiler(${PROJECT_NAME_LIB})

target_sources(${PROJECT_NAME_LIB}
    PRIVATE
        src/utils/process/process_unix.cpp
        src/utils/process/process.cpp
        src/utils/hash.cpp
        src/utils/utils.cpp
        src/utils/pattern.cpp
        src/memory/buffer.cpp
        src/memory/memory.cpp
        src/memory/stringfinder.cpp
        src/memory/program.cpp
        src/database/database.cpp
        src/surface/surface.cpp
        src/surface/renderer.cpp
        src/plugins/pluginmanager.cpp
        src/plugins/modulemanager.cpp
        src/typing/basetypes.cpp
        src/typing/typing.cpp
        src/disasm/worker.cpp
        src/disasm/emulator.cpp
        src/disasm/function.cpp
        src/disasm/memprocess.cpp
        src/graph/graph.cpp
        src/graph/datagraph.cpp
        src/graph/styledgraph.cpp
        src/graph/layouts/layeredlayout.cpp
        src/rdil/expression.cpp
        src/rdil/expressionlist.cpp
        src/rdil/rdil.cpp
        src/listing.cpp
        src/context.cpp
        src/state.cpp
        src/theme.cpp

    PRIVATE # Builtins
        src/builtins/loader.cpp
        src/builtins/processor.cpp
        src/builtins/analyzer.cpp

    PRIVATE # API
        src/api/redasm.cpp
        src/api/program.cpp
        src/api/marshal.cpp
        src/api/hash.cpp
        src/api/utils.cpp
        src/api/ui.cpp
        src/api/plugin.cpp
        src/api/byteorder.cpp
        src/api/buffer.cpp
        src/api/environment.cpp
        src/api/memory.cpp
        src/api/stream.cpp
        src/api/byte.cpp
        src/api/theme.cpp
        src/api/typing.cpp
        src/api/function.cpp
        src/api/listing.cpp
        src/api/analyzer.cpp
        src/api/loader.cpp
        src/api/surface.cpp
        src/api/processor.cpp
        src/api/renderer.cpp
        src/api/graph.cpp
        src/api/rdil.cpp
)

target_include_directories(${PROJECT_NAME_LIB} PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME_LIB}
    PRIVATE
        optional
        fmt
        miniz
        sqlite3
        spdlog::spdlog_header_only
)

add_library(${PROJECT_NAME}::lib ALIAS ${PROJECT_NAME_LIB})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(${PROJECT_NAME} src/main.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}::lib)
    setup_compiler(${PROJECT_NAME})
endif()

add_subdirectory(plugins)

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
