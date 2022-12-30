# 引入外部函数
include(${ROOT_DIR}/module_spdlog.cmake)

# 允许小项目分类
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# 生成项目
# ARGV0 ${AbsolutePathProject}  绝对路径
# ARGV1 ${Type}                 生成类型
macro(CreateTarget AbsolutePathProject Type)
    # 项目名称
    message(STATUS ${AbsolutePathProject})
    get_filename_component(PROJECT_NAME ${AbsolutePathProject} NAME)
    project(${PROJECT_NAME})

    # 将当前目录下所有源码文件添加到变量
    set(CURRENT_PATH ".")
    set(HEADER_FILES "")
    set(SOURCE_FILES "")
    file(GLOB HEADER_FILES "${CURRENT_PATH}/*.h" "${CURRENT_PATH}/*.hpp")
    file(GLOB SOURCE_FILES "${CURRENT_PATH}/*.c" "${CURRENT_PATH}/*.cpp")

    # 文件分类
    source_group("Header Files" FILES ${HEADER_FILES})
    source_group("Source Files" FILES ${SOURCE_FILES})

    # 头文件搜索的路径
    include_directories(${CURRENT_PATH})

    # 生成可执行文件
    add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

    # 添加 spdlog
    AddSpdlog()
endmacro()
