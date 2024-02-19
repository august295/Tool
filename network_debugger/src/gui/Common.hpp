#ifndef __COMMON_HPP__
#define __COMMON_HPP__

enum NetworkType
{
    ROOT_CLIENT,
    ROOT_SERVER,
    LEAF_CLIENT,
    LEAF_SERVER,
};

enum NetworkState
{
    CONNECT,
    DISCONNECT,
};

#endif // __COMMON_HPP__