# 线程池



## 1 项目介绍

该项目用于学习 `C++11` 多线程管理。

- 单线程
- 多线程无管理
- 线程池-任务并行
- 线程池-数据并行（未实现）



## 2 项目使用

### 2.0 项目环境

- `Visual Studio 2022` 
- `Cmake3.10.0`（及以上）

### 2.1 项目结构

```cpp
ThreadPool
|   .clang-format
|   .gitignore
|   cmake-vs2022.bat
|   CMakeLists.txt
|   module.cmake
|   README.md
|
\---src
    |   CMakeLists.txt
    |
    +---MultiThread
    |       CMakeLists.txt
    |       main.cpp
    |
    +---MultiThreadPool
    |       CMakeLists.txt
    |       main.cpp
    |       ThreadPool.cpp
    |       ThreadPool.h
    |
    \---SingleThread
            CMakeLists.txt
            main.cpp
            SingleThread.cpp
            SingleThread.h
```

### 2.2 项目生成

其中源代码在 `src` 中。

1. 采用 `cmake-vs2022.bat` 生成项目，或者使用 `cmake-gui` 图形界面生成项目，再用 `Visual Studio 2022` 打开



## 参考

[1] [Implement Thread Pool in C++ - sinkinben (cnblogs.com)](https://www.cnblogs.com/sinkinben/p/16064857.html)
