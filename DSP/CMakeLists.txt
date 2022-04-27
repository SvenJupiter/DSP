# define Library "DSP"
add_library(DSP SHARED "")
target_include_directories(DSP INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(DSP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(DSP PRIVATE -D BUILD_SHARDED_DSP_LIB=1)
target_compile_definitions(DSP INTERFACE -D USE_SHARDED_DSP_LIB=1)
target_compile_options(DSP PRIVATE -Werror -Wall -Wextra)
# target_compile_options(DSP PRIVATE -Wall)


# add library subfolders
cmake_policy(SET CMP0076 NEW)
add_subdirectory(src)
