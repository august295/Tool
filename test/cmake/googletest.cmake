set(BUILD_SHARED_LIBS ON)
add_subdirectory(submodules/googletest)
set_property(TARGET "gtest" PROPERTY FOLDER "googletest")
set_property(TARGET "gtest_main" PROPERTY FOLDER "googletest")
set_property(TARGET "gmock" PROPERTY FOLDER "googletest")
set_property(TARGET "gmock_main" PROPERTY FOLDER "googletest")

set(gtest_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/submodules/googletest/googletest/include)