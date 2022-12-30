# 引入外部函数
include(${ROOT_DIR}/module.cmake)

# 获取绝对路径
set(AbsolutePathProject ${CMAKE_CURRENT_SOURCE_DIR})

# 创建项目
CreateTarget(${AbsolutePathProject} "Exe")
