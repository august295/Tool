if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(SDL2_ROOT "D:\\ThirdPart\\SDL2-2.28.1")
endif()

macro(AddSDL2 ProjectName)
    include_directories(${SDL2_ROOT}/include)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(Prefix ${SDL2_ROOT}/lib/x64)
    else()
        set(Prefix ${SDL2_ROOT}/lib/x86)
    endif()

    target_link_libraries(${ProjectName} ${Prefix}/SDL2.lib)

    # copy dll
    add_custom_command(
        TARGET ${ProjectName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${Prefix}/SDL2.dll
        ${BUILD_DIR}/bin)
endmacro()
