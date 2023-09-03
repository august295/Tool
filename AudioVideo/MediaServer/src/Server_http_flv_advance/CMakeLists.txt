# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)
include(${ROOT_DIR}/cmake/module_boost.cmake)

# 引入三方库
set(ThirdPartyList "jsoncpp")
# 引入 boost
set(BoostLibraryList "system")

# 包含头文件
IncludeThirdParty(${ThirdPartyList})

# 创建项目
set(ProjectName "Server_http_flv_advance")
CreateTarget(${ProjectName} "Exe" "Server_advance")

# 链接库
LinkThirdParty(${ProjectName} "${ThirdPartyList}")
AddBoostLib(${ProjectName} "${BoostLibraryList}")