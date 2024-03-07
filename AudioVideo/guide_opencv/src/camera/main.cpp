#include <iostream>

#include <opencv2/opencv.hpp>

int main()
{
    // 默认获取第一个
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        // 处理无法打开摄像头的情况
        return -1;
    }
    cv::Mat frame;
    // 在这里对每一帧图像进行处理
    while (cap.read(frame))
    {
        // 显示图像
        cv::imshow("Camera", frame);
        // 按下ESC键退出循环
        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }
    cap.release();
    cv::destroyAllWindows();

    return 0;
}