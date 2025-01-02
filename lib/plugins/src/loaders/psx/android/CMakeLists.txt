project(LibDEX)

file(GLOB HEADERS rd_compat.h libdex/*.h)
file(GLOB SOURCES libdex/*.c*)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE ${REDASM_INCLUDE_PATH})
set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD_REQUIRED 17)
