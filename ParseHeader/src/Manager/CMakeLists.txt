﻿# 引入外部函数
include(${ROOT_DIR}/module.cmake)

# 获取文件名称
set(AbsolutePathProject ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(ProjectName ${AbsolutePathProject} NAME)
# 链接库
set(SELF_LIBRARY_LIST "ParseHeader")
# 预编译
add_compile_definitions(MANAGER_LIBRARY)

# 创建项目
CreateTarget(${ProjectName} "Dll")
