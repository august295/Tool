#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <algorithm>  // std::transform, std::find_if
#include <cctype>     // std::isspace
#include <cwctype>    // std::iswspace
#include <functional> // std::ptr_fun, std::not1
#include <iomanip>    // std::setfill, std::setw, std::setiosflags
#include <iostream>   // std::cout, std::endl
#include <map> // std::map
#include <sstream>  // std::ostringstream
#include <stdlib.h> // srand, rand
#include <string> // std::string
#include <time.h> // time

#include "dirent.h"

// convert to upper case
static inline std::string upper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// convert to lower case
static inline std::string lower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// trim leading spaces
static inline std::string& ltrim(std::string& str)
{
    // str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::cref(std::iswspace))));
    return str;
}

// trim trailing spaces
static inline std::string& rtrim(std::string& str)
{
    // str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
    str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::cref(std::iswspace))).base(), str.end());
    return str;
}

// trim spaces at both ends
static inline std::string& trim(std::string& str) { return ltrim(rtrim(str)); }

// convert binary data to hexadecimal string with hex prefix "0x"
static inline std::string tohex(std::string data, bool big_endian = false, bool upper_case = false)
{
    if (data.empty())
        return 0x00;

    std::ostringstream ret;
    ret << "0x";

    std::string::size_type len = data.length();
    unsigned int           byte;

    for (std::string::size_type i = 0; i < len; ++i)
    {
        byte = (big_endian ? data[i] : data[len - i - 1]) & 0xff;

        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase)
            << byte;
    }

    return ret.str();
}

static inline std::string rands()
{
    srand(static_cast<unsigned int>(time(NULL)));

    std::ostringstream os;
    os << rand();

    return os.str();
}

// 显示日志
enum LogLevels
{
    kError,
    kDebug,
    kInfo,
};
static LogLevels g_log_level = kInfo;

static void Log(enum LogLevels level, std::string msg)
{
    if (level > g_log_level)
        return;

    std::ostringstream os;
    switch (level)
    {
    case kError: os << "ERROR: "; break;
    case kDebug: os << "DEBUG: "; break;
    default: os << "INFO: ";
    }

    os << msg;
    std::cout << os.str() << std::endl;
}

// 日志函数
static void Error(std::string msg) { Log(kError, msg); }
static void Debug(std::string msg) { Log(kDebug, msg); }
static void Info(std::string msg) { Log(kInfo, msg); }

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	获取路径下所有 .h 文件. </summary>
///
/// <remarks>	李俊峰, 2022/9/6. </remarks>
///
/// <param name="folder">	文件夹路径. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
static void FindFiles(const std::string& folder, std::map<std::string, std::map<std::string, bool>>& fileMap)
{
    DIR*           dir;
    struct dirent* ent;
    struct stat    entrystat;

    if ((dir = opendir(folder.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string name(ent->d_name);
            if (name.compare(".") == 0 || name.compare("..") == 0)
                continue;

            name = folder + "/" + name;
            if (0 == stat(name.c_str(), &entrystat))
            {
                if (S_ISDIR(entrystat.st_mode))
                {
                    Info("Searching folder: " + name);
                    FindFiles(name, fileMap);
                }
                else
                {
                    if (name.length() > 2 && name.substr(name.length() - 2, 2).compare(".h") == 0)
                    {
                        fileMap.emplace(name, std::map<std::string, bool>());
                        Debug("Found header file: " + name);
                    }
                    else
                    {
                        Info("Ignoring file: " + name);
                    }
                }
            }
            else
            {
                Error("failed to stat file/folder: " + name);
            }
        }
        closedir(dir);
    }
    else
    {
        Error("failed to open folder: " + folder);
    }
}

#endif // _UTILITY_H_
