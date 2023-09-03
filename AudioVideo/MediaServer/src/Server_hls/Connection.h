//
// Created by bxc on 2023/1/7.
//

#ifndef BXC_HLSSERVER_CONNECTION_H
#define BXC_HLSSERVER_CONNECTION_H

#include <mutex>
#include <queue>
#include <vector>

class Connection
{
public:
    Connection(int clientFd);
    ~Connection();

public:
    int start();

private:
    int mClientFd;
};

#endif // BXC_HLSSERVER_CONNECTION_H