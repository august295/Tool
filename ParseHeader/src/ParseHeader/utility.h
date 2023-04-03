#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <algorithm>  // std::transform, std::find_if
#include <cctype>     // std::isspace
#include <functional> // std::ptr_fun, std::not1
#include <iomanip>    // std::setfill, std::setw, std::setiosflags
#include <iostream>   // std::cout, std::endl
#include <sstream>    // std::ostringstream
#include <stdlib.h>   // srand, rand
#include <string>     // std::string
#include <time.h>     // time

// convert to upper case
inline std::string upper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// convert to lower case
inline std::string lower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// trim leading spaces
inline std::string& ltrim(std::string& str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return str;
}

// trim trailing spaces
inline std::string& rtrim(std::string& str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
    return str;
}

// trim spaces at both ends
inline std::string& trim(std::string& str) { return ltrim(rtrim(str)); }

// convert binary data to hexadecimal string with hex prefix "0x"
inline std::string tohex(std::string data, bool big_endian = false, bool upper_case = false)
{
    if (data.empty())
        return 0x00;

    std::ostringstream ret;
    ret << "0x";

    std::string::size_type len = data.length();
    unsigned int           byte;

    for (std::string::size_type i = 0; i < len; ++i) {
        byte = (big_endian ? data[i] : data[len - i - 1]) & 0xff;

        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase)
            << byte;
    }

    return ret.str();
}

inline std::string rands()
{
    srand(static_cast<unsigned int>(time(NULL)));
    std::ostringstream os;
    os << rand();
    return os.str();
}

// mini log
enum LogLevels {
    kNone,
    kError,
    kDebug,
    kInfo,
};
static LogLevels g_log_level = kError;

void Log(std::string file, int line, enum LogLevels level, std::string msg)
{
    if (level > g_log_level)
        return;

    std::ostringstream os;
    switch (level) {
    case kError: os << "[" << file << ":" << line << "] [ERROR]: "; break;
    case kDebug: os << "[" << file << ":" << line << "] [DEBUG]: "; break;
    case kInfo: os << "[" << file << ":" << line << "] [kInfo]: "; break;
    case kNone:
    default: break;
    }
    os << msg;
    std::cout << os.str() << std::endl;
}

#define LOG_ERROR(msg) Log(__FILE__, __LINE__, kError, msg)
#define LOG_DEBUG(msg) Log(__FILE__, __LINE__, kDebug, msg)
#define LOG_INFO(msg)  Log(__FILE__, __LINE__, kInfo, msg)

#endif // _UTILITY_H_
