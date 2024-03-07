if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(FFMPEG_ROOT "D:/Video/ffmpeg-6.0-full_build-shared")
endif()

macro(AddFFmpeg ProjectName FFmpegLibs)
    include_directories(${FFMPEG_ROOT}/include)

    foreach(lib ${FFmpegLibs})
        target_link_libraries(${ProjectName} ${FFMPEG_ROOT}/lib/${lib}.lib)

        # copy dll
        file(GLOB ffmpeg_dlls ${FFMPEG_ROOT}/bin/*.dll)

        foreach(dll ${ffmpeg_dlls})
            get_filename_component(dll_name ${dll} NAME)

            if(${dll_name} MATCHES ${lib})
                add_custom_command(
                    TARGET ${ProjectName} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${dll}
                    ${BUILD_DIR}/bin)
            endif()
        endforeach()
    endforeach()
endmacro()
