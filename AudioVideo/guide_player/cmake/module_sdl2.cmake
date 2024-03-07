if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(SDL2_DIR "D:/Video/SDL2-2.28.1/cmake")
endif()

set(CMAKE_PREFIX_PATH ${SDL2_DIR})

macro(AddSDL2 ProjectName)
    # 查找 SDL2 库
    find_package(SDL2 REQUIRED)

    # 链接 SDL2 库
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(${ProjectName} ${SDL2_LIBRARIES})

    # copy dll
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        file(GLOB sdl2_dlls ${SDL2_BINDIR}/*.dll)
        add_custom_command(
            TARGET ${ProjectName} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${sdl2_dlls}
            ${BUILD_DIR}/bin)
    endif()
endmacro()
