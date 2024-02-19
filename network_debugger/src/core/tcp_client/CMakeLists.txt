# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 预编译
add_compile_definitions(TCP_CLIENT_EXPORT)
# Qt 库
set(QT_LIBRARY_LIST "Core" "Network")
# 三方库
set(LibraryHeaderOnlyList "spdlog")

# 创建项目
CreateTarget("tcp_client" "Dll" "core")
