#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "spdlog/spdlog.h"

#define SER_PORT 6666
#define MAX_LINE 1024

int getfd_info(int socket_fd, char* fd_ip, int* fd_port)
{
    struct sockaddr_storage storage;                    // 能够适应不同种类的地址协议结构
    socklen_t               sock_len = sizeof(storage); // 必须给初值

    int ret = getpeername(socket_fd, (struct sockaddr*)&storage, &sock_len);
    if (ret < 0)
    {
        return -1;
    }

    if (storage.ss_family == AF_INET)
    {
        struct sockaddr_in* addr = (struct sockaddr_in*)&storage;
        strcpy(fd_ip, inet_ntoa(addr->sin_addr));
        *fd_port = ntohs(addr->sin_port);
    }
    else if (storage.ss_family == AF_INET6)
    {
        struct sockaddr_in6* addr = (struct sockaddr_in6*)&storage;
        char                 ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(addr));
        strcpy(fd_ip, ip);
        *fd_port = ntohs(addr->sin6_port);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::trace);
    int listen_fd       = 0;   // 监听描述符
    int ret             = 0;   // 返回值
    int connect_fd[100] = {0}; // 为 0 的成员为无效socket
    int connect_cnt     = 0;   // 连接数量

    char buf[MAX_LINE]   = {0}; // 数据长度
    char fd_ip[MAX_LINE] = {0}; // 连接 IP
    int  fd_port         = 0;   // 连接端口

    // 创建套接字
    listen_fd = socket(AF_INET, SOCK_STREAM, 0); // IPV4, TCP socket
    if (listen_fd == -1)
    {
        SPDLOG_ERROR("Socket create failed");
    }

    // 指定地址
    struct sockaddr_in ser_addr;
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family      = AF_INET;           // 地址协议族
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定 IP 地址 inet_addr("127.0.0.1")
    ser_addr.sin_port        = htons(SER_PORT);   // 指定端口号

    // 服务器绑定
    bind(listen_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));

    ret = listen(listen_fd, 10);
    if (ret == -1)
    {
        SPDLOG_ERROR("Listen create failed.");
    }
    spdlog::info("Listening...");

    int    max_fd = listen_fd; // 一开始时，只有 一个新的 文件描述：listen_fd ，所以它是最大的
    fd_set read_sets;          // 读取集合（收）
    // 在循环中等待连接请求
    while (1)
    {
        FD_ZERO(&read_sets);           // 每次都需要初始化
        FD_SET(listen_fd, &read_sets); // 添加要监听的 socket

        // 添加之后经过 connect 过来的 套接字数组(一般在第一次循环时是空的)
        for (int i = 0; i < connect_cnt; i++)
        {
            if (connect_fd[i])
            {
                FD_SET(connect_fd[i], &read_sets); // 添加经过accept保存下来，需要进行读响应的套接字到集合中
            }
        }

        // 设置监听超时时间
        struct timeval timeout;
        timeout.tv_sec  = 30;
        timeout.tv_usec = 0;

        ret = select(max_fd + 1, &read_sets, NULL, NULL, &timeout);
        switch (ret)
        {
        case -1: SPDLOG_ERROR("Select error."); break; // 监听错误
        case 0: SPDLOG_INFO("No active fd"); continue; // 无活动文件描述符
        default:
            if (FD_ISSET(listen_fd, &read_sets)) // 这个是原的被动socket，如果是它，则 意味着有新的连接进来了
            {
                max_fd                  = accept(listen_fd, NULL, NULL);
                connect_fd[connect_cnt] = max_fd;
                connect_cnt++;

                memset(fd_ip, '\0', MAX_LINE);
                getfd_info(max_fd, fd_ip, &fd_port);
                spdlog::info("{}:{} connect", fd_ip, fd_port);
            }
            else
            {
                // 如果不是 被动socket，那么就意味着是 现有的连接 有消息发来（我们有数据可读）
                // 求出是那个文件描述符可读
                int i = 0;
                for (i = 0; i < connect_cnt; i++)
                {
                    if (FD_ISSET(connect_fd[i], &read_sets) == 1)
                        break;
                }
                if (i >= connect_cnt)
                {
                    continue;
                }

                // 接收消息
                memset(buf, '\0', MAX_LINE);
                memset(fd_ip, '\0', MAX_LINE);
                getfd_info(connect_fd[i], fd_ip, &fd_port);
                ret = recv(connect_fd[i], buf, MAX_LINE, 0);
                if (ret <= 0)
                {
                    // 远程客户端断开处理（如果不处理，会导致服务器也断开）
                    spdlog::info("{}:{} disconnect", fd_ip, fd_port);
                    close(connect_fd[i]);
                    // 我们需要将对应的客户端从数组中移除且连接数 -1 (移除的方法： 数组成员前移覆盖)
                    for (int j = i; j < connect_cnt - 1; ++j)
                    {
                        connect_fd[j] = connect_fd[j + 1];
                    }
                    connect_cnt--;
                    break;
                }

                // 打印消息
                spdlog::trace("{}:{} {}", fd_ip, fd_port, buf);
                send(connect_fd[i], buf, ret, 0);
            }
            break;
        }
    }

    // 关闭连接
    for (int i = 0; i < connect_cnt; i++)
    {
        close(connect_fd[i]);
    }
    close(listen_fd);

    return 0;
}