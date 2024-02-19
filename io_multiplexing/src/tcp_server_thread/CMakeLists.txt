# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 创建项目
CreateTarget("tcp_server_thread" "Exe" "server")
