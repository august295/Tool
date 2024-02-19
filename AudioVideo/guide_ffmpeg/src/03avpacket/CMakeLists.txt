# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)
include(${ROOT_DIR}/cmake/module_ffmpeg.cmake)

# 创建项目
set(ProjectName "03avpacket")
CreateTarget(${ProjectName} "Exe" "api_base")

set(FFmpegList avcodec avdevice avfilter avformat avutil postproc swscale swresample)
AddFFMPEG(${ProjectName} "${FFmpegList}")
