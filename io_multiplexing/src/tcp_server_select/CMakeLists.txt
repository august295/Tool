# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 仅头文件三方库
set(3rdPartyHeads "spdlog")
IncludeThirdParty("${3rdPartyHeads}")

# 创建项目
CreateTarget("tcp_server_select" "Exe" "server")
