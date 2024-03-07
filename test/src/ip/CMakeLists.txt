set(ProjectName "ip")
CreateTarget(${ProjectName} "Exe" "ip")

include_directories(${gtest_include_dir})
target_link_libraries(${ProjectName} gtest)