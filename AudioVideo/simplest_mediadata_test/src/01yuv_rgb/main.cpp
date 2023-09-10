#include <iostream>
#include <string>

#include "yuv_rgb.h"

int main(int argc, char* argv[])
{
    std::string filepath        = "../../resources/";
    std::string yuv420p         = filepath + "lena_256x256_yuv420p.yuv";
    std::string yuv420p_distort = filepath + "lena_distort_256x256_yuv420p.yuv";
    std::string yuv422p         = filepath + "lena_256x256_yuv422p.yuv";
    std::string yuv444p         = filepath + "lena_256x256_yuv444p.yuv";
    std::string rgb_cie1931     = filepath + "cie1931_500x500.rgb";
    std::string rgb_lena        = filepath + "lena_256x256_rgb24.rgb";

    // 分离YUV420P像素数据中的Y、U、V分量
    simplest_yuv420_split(yuv420p.c_str(), 256, 256, 1);

    // 分离YUV444P像素数据中的Y、U、V分量
    simplest_yuv444_split(yuv444p.c_str(), 256, 256, 1);

    // 将YUV420P像素数据去掉颜色（变成灰度图）
    simplest_yuv420_gray(yuv420p.c_str(), 256, 256, 1);

    //  将YUV420P像素数据的亮度减半
    simplest_yuv420_halfy(yuv420p.c_str(), 256, 256, 1);

    // 将YUV420P像素数据的周围加上边框
    simplest_yuv420_border(yuv420p.c_str(), 256, 256, 20, 1);

    // 生成YUV420P格式的灰阶测试图
    simplest_yuv420_graybar(640, 360, 0, 255, 10, "graybar_640x360.yuv");

    // 计算两个YUV420P像素数据的PSNR
    simplest_yuv420_psnr(yuv420p.c_str(), yuv420p_distort.c_str(), 256, 256, 1);

    // 分离RGB24像素数据中的R、G、B分量
    simplest_rgb24_split(rgb_cie1931.c_str(), 500, 500, 1);

    // 将RGB24格式像素数据封装为BMP图像
    simplest_rgb24_to_bmp(rgb_lena.c_str(), 256, 256, "output_lena.bmp");

    // 将RGB24格式像素数据转换为YUV420P格式像素数据
    simplest_rgb24_to_yuv420(rgb_lena.c_str(), 256, 256, 1, "output_lena.yuv");

    // 生成RGB24格式的彩条测试图
    simplest_rgb24_colorbar(640, 360, "colorbar_640x360.rgb");

    return 0;
}