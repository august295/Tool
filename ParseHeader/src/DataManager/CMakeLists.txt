# 引入外部函数
include(${ROOT_DIR}/module.cmake)

# 获取绝对路径
set(AbsolutePathProject ${CMAKE_CURRENT_SOURCE_DIR})
# 链接库
set(SELF_LIBRARY_LIST ParseHeader)
# 创建项目
CreateTarget(${AbsolutePathProject} "Dll" "${SELF_LIBRARY_LIST}")
# 预编译
add_compile_definitions(DATAMANAGER_LIBRARY)
