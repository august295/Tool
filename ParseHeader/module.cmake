# 允许小项目分类
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

macro(CreateTarget AbsolutePathProject Type)
    # 引入 qt 宏
    if(NOT("${QT_LIBRARY_LIST}" STREQUAL ""))
        include(${ROOT_DIR}/module_qt.cmake)
    endif()

    # 项目名称
    message(STATUS ${AbsolutePathProject})
    get_filename_component(PROJECT_NAME ${AbsolutePathProject} NAME)
    project(${PROJECT_NAME})

    # 将当前目录下所有源码文件添加到变量
    set(CURRENT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    set(HEADER_FILES "")
    set(SOURCE_FILES "")
    set(FORM_FILES "")
    set(RESOURCE_FILES "")
    file(GLOB HEADER_FILES "${CURRENT_PATH}/*.h" "${CURRENT_PATH}/*.hpp")
    file(GLOB SOURCE_FILES "${CURRENT_PATH}/*.c" "${CURRENT_PATH}/*.cpp")
    file(GLOB FORM_FILES "${CURRENT_PATH}/*.ui")
    file(GLOB RESOURCE_FILES "${CURRENT_PATH}/*.qrc")

    # 文件分类
    source_group("Header Files" FILES ${HEADER_FILES})
    source_group("Source Files" FILES ${SOURCE_FILES})
    source_group("Form Files" FILES ${FORM_FILES})
    source_group("Resource Files" FILES ${RESOURCE_FILES})

    # 头文件搜索的路径
    include_directories(${CURRENT_PATH})
    include_directories(${ROOT_DIR}/include)

    # 添加 qt 头文件
    if(NOT("${QT_LIBRARY_LIST}" STREQUAL ""))
        AddQtInc("${QT_LIBRARY_LIST}" "${FORM_FILES}" "${RESOURCE_FILES}")
    endif()

    if(${Type} STREQUAL "Exe")
        # 生成可执行文件
        add_executable(${PROJECT_NAME}
            WIN32
            ${HEADER_FILES} ${SOURCE_FILES} ${FORM_FILES} ${RESOURCE_FILES})
        set_target_properties(${PROJECT_NAME} PROPERTIES
            DEBUG_POSTFIX "d")
    else()
        # 生成链接库
        if(${Type} STREQUAL "Lib")
            add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${FORM_FILES} ${RESOURCE_FILES})
        elseif(${Type} STREQUAL "Dll")
            add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${FORM_FILES} ${RESOURCE_FILES})
        endif()
    endif()

    # 添加 qt 链接库
    if(NOT("${QT_LIBRARY_LIST}" STREQUAL ""))
        AddQtLib("${QT_LIBRARY_LIST}")
    endif()

    # 添加项目生成的链接库F
    foreach(_lib ${SELF_LIBRARY_LIST})
        include_directories(${CURRENT_PATH}/../)
        target_link_libraries(${PROJECT_NAME} ${_lib})
    endforeach()
endmacro()
