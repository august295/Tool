# 创建项目
set(ProjectName "02audio_player")
CreateTarget(${ProjectName} "Exe" "player")

AddSDL2(${ProjectName})
set(FFmpegList "avcodec" "avdevice" "avfilter" "avformat" "avutil" "postproc" "swscale" "swresample")
AddFFMPEG(${ProjectName} "${FFmpegList}")
