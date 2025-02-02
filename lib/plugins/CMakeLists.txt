cmake_minimum_required(VERSION 3.25)

project(plugins)

include(cmake/Dependencies.cmake)
setup_dependencies()

include(cmake/Compiler.cmake)

# Loaders
# add_subdirectory(src/loaders/dos)
# add_subdirectory(src/loaders/elf)
add_subdirectory(src/loaders/psx)
# add_subdirectory(src/loaders/android)

# Processors
# add_subdirectory(src/processors/capstonebundle)
# add_subdirectory(src/processors/x86)
# add_subdirectory(src/processors/mips)

# Plugins
# add_subdirectory(src/python)
