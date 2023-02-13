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
    set(CURRENT_PATH ".")
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
#    include_directories(${ROOT_DIR}/include)

    # 添加 qt 头文件
    if(NOT("${QT_LIBRARY_LIST}" STREQUAL ""))
        AddQtInc("${QT_LIBRARY_LIST}" "${FORM_FILES}" "${RESOURCE_FILES}")
    endif()

    if(${Type} STREQUAL "Exe")
        set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
        # 生成可执行文件
        add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${FORM_FILES} ${RESOURCE_FILES})
    else()
        # 生成链接库
        if(${Type} STREQUAL "Lib")
            add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${FORM_FILES} ${RESOURCE_FILES})
        elseif(${Type} STREQUAL "Dll")
            add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${FORM_FILES} ${RESOURCE_FILES})
        endif()
#        # 生成事件
#        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
#            make_directory ${ROOT_DIR}/include/${PROJECT_NAME}
#        )
#        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
#            copy ${HEADER_FILES} ${ROOT_DIR}/include/${PROJECT_NAME}
#        )
    endif()

    # 添加 qt 链接库
    if(NOT("${QT_LIBRARY_LIST}" STREQUAL ""))
        AddQtLib("${QT_LIBRARY_LIST}")
    endif()

    if(${ARGC} GREATER_EQUAL 3)
        include_directories(${CURRENT_PATH}/../)
        link_directories(${BUILD_DIR}/lib)
        target_link_libraries(${PROJECT_NAME} ${ARGV2})
    endif()
endmacro()
