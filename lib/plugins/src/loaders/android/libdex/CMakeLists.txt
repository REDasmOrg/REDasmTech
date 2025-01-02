project(libdex)

add_library(${PROJECT_NAME} STATIC 
    src/libdex/DexCatch.cpp
    src/libdex/DexFile.cpp 
    src/libdex/DexSwapVerify.cpp
    src/libdex/sha1.cpp
    src/libdex/DexClass.cpp
    src/libdex/DexOpcodes.cpp
    src/libdex/DexUtf.cpp
    src/libdex/DexDataMap.cpp
    src/libdex/DexOptData.cpp
    src/libdex/InstrUtils.cpp
    src/libdex/DexDebugInfo.cpp
    src/libdex/DexProto.cpp
    src/libdex/Leb128.cpp
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/src"

    PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/libdex"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    redasm::lib
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES 
        C_STANDARD_REQUIRED 17
)

