# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# Qt 库
set(QT_LIBRARY_LIST "Core" "Gui" "Widgets" "Network")
# 三方库
set(LibraryHeaderOnlyList "spdlog" "nlohmann")
# 链接库
set(SELF_LIBRARY_LIST "tcp_client")

# 创建项目
CreateTarget("network_debugger" "Exe" "gui")
