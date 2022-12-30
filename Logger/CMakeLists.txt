# 要求最低Cmake版本
cmake_minimum_required(VERSION 3.10.0)

# 项目名称
project(Logger)

# C++标准
set(CMAKE_CXX_STANDARD 11)
# MSVC 强制使用 utf-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 设置根目录
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${ROOT_DIR}/build)

# 输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR}/lib/debug) # .lib and .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR}/lib/debug) # .so and .dylib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR}/bin/debug) # .exe and .dll
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR}/lib/release) # .lib and .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR}/lib/release) # .so and .dylib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR}/bin/release) # .exe and .dll
set(CMAKE_DEBUG_POSTFIX "d")

# 添加子目录，子目录下必须有CMakeLists.txt
add_subdirectory(src)
