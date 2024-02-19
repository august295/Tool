#ifndef TCP_CLIENT_EXPORT_HPP
#define TCP_CLIENT_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
    #ifdef TCP_CLIENT_EXPORT
        #define TCP_CLIENT_API __declspec(dllexport)
    #else
        #define TCP_CLIENT_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define TCP_CLIENT_API __attribute__((visibility("default")))
#else
    #define TCP_CLIENT_API
#endif

#endif
