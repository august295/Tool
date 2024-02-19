# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)
include(${ROOT_DIR}/cmake/module_sdl2.cmake)
include(${ROOT_DIR}/cmake/module_ffmpeg.cmake)

# 创建项目
set(ProjectName "01player")
CreateTarget(${ProjectName} "Exe" "player")

AddSDL2(${ProjectName})
set(FFmpegList "avformat" "avcodec" "swscale")
AddFFMPEG(${ProjectName} "${FFmpegList}")
