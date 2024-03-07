if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(OpenCV_DIR "D:/Video/opencv/build/")
endif()

set(CMAKE_PREFIX_PATH ${OpenCV_DIR})

macro(AddOpencv ProjectName)
    # 查找 OpenCV 库
    find_package(OpenCV REQUIRED)

    # 链接 OpenCV 库
    target_link_libraries(${ProjectName} ${OpenCV_LIBS})

    # copy dll
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        file(GLOB opencv_dlls ${_OpenCV_LIB_PATH}/*.dll)
        add_custom_command(
                TARGET ${ProjectName} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${opencv_dlls}
                ${BUILD_DIR}/bin)
    endif()
endmacro()
