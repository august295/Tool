# 头文件解析



## 1 项目介绍

### 1.0 基础

该项目是基于 [fanghm/c-header-parser](https://github.com/fanghm/c-header-parser) 实现，核心已实现解析 `C/C++` 头文件，添加 `QT` 界面。

### 1.1 功能

该项目用于解析 `C/C++` 头文件并展示，允许添加新结构体，包括

- 常量，数值宏定义

- 枚举
- 结构体，联合体（可显示注释）



## 2 项目使用

### 2.0 项目环境

- `Visual Studio 2017` 或者 `Qt Creator4` （至少支持 C++11）
- `Cmake3.10.0`（及以上）

### 2.1 项目结构

```cpp
ParseHeader
|
|   cmake-vs2017.bat
|   CMakeLists.txt
|   module.cmake
|   module_qt.cmake
|   README.md
|   
\---src
    |   CMakeLists.txt
    |
    +---MainWidget
    |       CMakeLists.txt
    |       DelegateComboBox.cpp
    |       DelegateComboBox.h
    |       FileManage.cpp
    |       FileManage.h
    |       main.cpp
    |       ParseHeaderWidget.cpp
    |       ParseHeaderWidget.h
    |       ParseHeaderWidget.ui
    |       StructOperateWidget.cpp
    |       StructOperateWidget.h
    |       StructOperateWidget.ui
    |
    +---ParseHeader
    |       CMakeLists.txt
    |       defines.h
    |       dirent.h
    |       TypeParser.cpp
    |       TypeParser.h
    |       utility.h
    |
    \---resources
            Employee.h
            struct.h
```

### 2.2 项目生成

其中源代码在 `src` 中，测试头文件在 `resources` 中。

1. 使用 `Qt Creator4` 导入 `src/CMakeLists.txt`
2. 采用 `cmake-vs2017.bat` 生成项目，或者使用 `cmake-gui` 图形界面生成项目，再用 `Visual Studio 2017` 打开

### 2.3 项目文件

| 文件名              | 作用                                               |
| ------------------- | -------------------------------------------------- |
| dirent              | 三方库源代码文件，解析文件路径并                   |
| utility             | 通用函数文件，包括编码转换、转换大小写、去除空格等 |
| defines             | 定义文件，包括结构体解析结构体、数据标记、符号标记 |
| TypeParser          | 解析头文件核心文件                                 |
|                     |                                                    |
| ParseHeaderWidget   | 主界面文件                                         |
| StructOperateWidget | 添加结构体界面文件                                 |
| DelegateComboBox    | 插入表格的下拉框代理文件                           |
| FileManage          | 文件管理，获取文件内容和保存修改文件               |

每个文件中都有详细的注释，以供使用和修改。

读取的文件路径是 `./src/resources` 下的头文件，可以在 `Parse_CHeader_Qt.cpp` 中 59 行进行修改。

```cpp
void ParseHeaderWidget::ParseFiles()
{
	// 解析指定文件夹下头文件
	std::set<std::string> inc_paths;
	inc_paths.insert("../../src/resources");

	for (auto path : inc_paths)
	{
		FindFiles(path, _p->_FileMap);
		for (auto file : _p->_FileMap)
		{
			std::string filename = file.first;
			TypeParser parser;
			parser.ParseFile(filename);
			_p->_TypeParserMap.emplace(filename, parser);
		}
	}
}
```



## 参考

[1] [fanghm/c-header-parser](https://github.com/fanghm/c-header-parser)
