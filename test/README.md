# network_debugger

这是一个 `Qt` 编写的网络调试助手



## 1. 环境

- `Qt5.14.2`
- `CMake3.15`
- `QtCreator 4.11` 或 `Visual Studio 2017`



## 2. 编译

如果 `Qt` 版本不同需要修改 `./cmake/module_qt.cmake` 中设置的 `Qt` 查找路径

- `QtCreator 4.11` 直接将 `CMakeLists.txt` 拖入即可生成项目
- `Visual Studio 2017` 使用 `cmake_vs2017.bat` 脚本即可生成



## 3. 功能

- [x] 实现 `tcp client` 连接
