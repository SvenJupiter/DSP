
# Build DSPc as a static library
option(BUILD_STATIC_DSP_LIB "Build DSPc as a static library" OFF)

if (BUILD_STATIC_DSP_LIB)

    # define Library "DSPc"
    add_library(DSPc STATIC "")
    target_include_directories(DSPc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(DSPc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_options(DSPc PRIVATE -Werror -Wall -Wextra)
    # target_compile_options(DSPc PRIVATE -Wall)

else()

    # define Library "DSPc"
    add_library(DSPc SHARED "")
    target_include_directories(DSPc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(DSPc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_definitions(DSPc PRIVATE -D BUILD_SHARDED_DSP_LIB=1)
    target_compile_definitions(DSPc INTERFACE -D USE_SHARDED_DSP_LIB=1)
    target_compile_options(DSPc PRIVATE -Werror -Wall -Wextra)
    # target_compile_options(DSPc PRIVATE -Wall)

endif()

# add library subfolders
cmake_policy(SET CMP0076 NEW)
add_subdirectory(src)
