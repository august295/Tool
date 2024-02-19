#include <iostream>

#ifdef __cplusplus
extern "C"
{
    #include <libavformat/avformat.h>
}
#endif // __cplusplus

int main(int argc, char* argv[])
{
    AVFormatContext* fmt_ctx = NULL;
    int              type    = 2;
    int              err;
    const char*      filename = "../../resources/SampleVideo_1280x720_1mb.mp4";

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx)
    {
        printf("error code: %d\n", AVERROR(ENOMEM));
        return -1;
    }

    if (1 == type)
    {
        if ((err = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0)
        {
            printf("error code: %d\n", err);
        }
        else
        {
            printf("open success\n");
            printf("filename - %s\n", fmt_ctx->url);
            printf("duration - %I64d\n", fmt_ctx->duration);
            printf("nb_streams - %u\n", fmt_ctx->nb_streams);
            for (int i = 0; i < fmt_ctx->nb_streams; i++)
            {
                printf("stream codec_type - %d\n", fmt_ctx->streams[i]->codecpar->codec_type);
            }
            printf("iformat name - %s\n", fmt_ctx->iformat->name);
            printf("iformat long name - %s\n", fmt_ctx->iformat->long_name);
        }
    }
    else if (2 == type)
    {
        // Set probe size
        AVDictionary* format_opts = NULL;
        av_dict_set(&format_opts, "probesize", "1024", 0);

        if ((err = avformat_open_input(&fmt_ctx, filename, NULL, &format_opts)) < 0)
        {
            printf("error code %d \n", err);
        }
        else
        {
            avformat_find_stream_info(fmt_ctx, NULL);
            printf("open success \n");
            printf("filename - %s \n", fmt_ctx->url);
            printf("duration - %I64d \n", fmt_ctx->duration);
            printf("nb_streams - %u \n", fmt_ctx->nb_streams);
            for (int i = 0; i < fmt_ctx->nb_streams; i++)
            {
                printf("stream codec_type - %d \n", fmt_ctx->streams[i]->codecpar->codec_type);
            }
            printf("iformat name - %s \n", fmt_ctx->iformat->name);
            printf("iformat long name - %s \n", fmt_ctx->iformat->long_name);
        }
        av_dict_free(&format_opts);
    }

    return 0;
}