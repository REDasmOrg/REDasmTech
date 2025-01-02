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
        src/memory/abstractbuffer.cpp
        src/memory/abstractstream.cpp
        src/memory/memorystream.cpp
        src/memory/file.cpp
        src/memory/memory.cpp
        src/memory/stream.cpp
        src/memory/stringfinder.cpp
        src/database/database.cpp
        src/surface/surface.cpp
        src/surface/renderer.cpp
        src/modulemanager.cpp
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
        src/segment.cpp
        src/theme.cpp

    PRIVATE # Builtins
        src/builtins/loader.cpp
        src/builtins/processor.cpp
        src/builtins/analyzer.cpp

    PRIVATE # Internal
        src/api/marshal.cpp
        src/api/internal/hash.cpp
        src/api/internal/utils.cpp
        src/api/internal/redasm.cpp
        src/api/internal/byteorder.cpp
        src/api/internal/rdil.cpp
        src/api/internal/byte.cpp
        src/api/internal/loader.cpp
        src/api/internal/analyzer.cpp
        src/api/internal/processor.cpp
        src/api/internal/theme.cpp
        src/api/internal/typing.cpp
        src/api/internal/buffer.cpp
        src/api/internal/file.cpp
        src/api/internal/memory.cpp
        src/api/internal/stream.cpp
        src/api/internal/listing.cpp
        src/api/internal/surface.cpp
        src/api/internal/renderer.cpp
        src/api/internal/graph.cpp
        src/api/internal/function.cpp

    PRIVATE # C
        src/api/c/hash.cpp
        src/api/c/utils.cpp
        src/api/c/redasm.cpp
        src/api/c/byteorder.cpp
        src/api/c/buffer.cpp
        src/api/c/memory.cpp
        src/api/c/stream.cpp
        src/api/c/byte.cpp
        src/api/c/theme.cpp
        src/api/c/typing.cpp
        src/api/c/function.cpp
        src/api/c/listing.cpp
        src/api/c/analyzer.cpp
        src/api/c/loader.cpp
        src/api/c/surface.cpp
        src/api/c/processor.cpp
        src/api/c/renderer.cpp
        src/api/c/graph.cpp
        src/api/c/rdil.cpp

    PRIVATE # Python
        src/api/python/utils.cpp
        src/api/python/byteorder.cpp
        src/api/python/rdil.cpp
        src/api/python/buffer.cpp
        src/api/python/stream.cpp
        src/api/python/common.cpp
        src/api/python/module.cpp
        src/api/python/typing.cpp
        src/api/python/redasm.cpp
)

target_include_directories(${PROJECT_NAME_LIB} PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME_LIB}
    PRIVATE
        expected
        optional
        fmt
        miniz
        sqlite3
        spdlog::spdlog_header_only
        Python3::Python
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
