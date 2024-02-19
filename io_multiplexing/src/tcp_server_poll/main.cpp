#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_SOCKET 5
#define SER_PORT   6666
#define MAX_LINE   1024

void getfd_info(int socket_fd, char* fd_ip, int* fd_port)
{
    struct sockaddr_storage storage;                    // 能够适应不同种类的地址协议结构
    socklen_t               sock_len = sizeof(storage); // 必须给初值

    int ret = getpeername(socket_fd, (struct sockaddr*)&storage, &sock_len);
    if (ret < 0)
    {
        printf("getpeername error\n");
        return;
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
}

int main(int argc, char* argv[])
{
    int listen_fd;
    int connect_fd[MAX_SOCKET];
    int connect_cnt = 0;

    char buf[MAX_LINE]   = {0}; // 数据长度
    char fd_ip[MAX_LINE] = {0}; // 连接 IP
    int  fd_port         = 0;   // 连接端口

    // 1 创建一个套接字，用于网络通信
    listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2 绑定服务的IP与端口
    struct sockaddr_in ser_addr;
    ser_addr.sin_family      = PF_INET;
    ser_addr.sin_port        = htons(SER_PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定 IP 地址 inet_addr("127.0.0.1")

    int ret = bind(listen_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }

    // 3 监听端口的数据
    ret = listen(listen_fd, MAX_SOCKET);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }
    printf("Listening\n");

    // 监听的总套接字数组
    struct pollfd pollfds[MAX_SOCKET + 1];
    int           poll_ret;

    // 主套接字信息
    pollfds[0].fd     = listen_fd;
    pollfds[0].events = POLLIN | POLLPRI; // 设置为 任意优先级可读 事件监听

    while (1)
    {
        poll_ret = poll(pollfds, connect_cnt + 1, -1); //-1 阻塞模式进行监听
        if (poll_ret == 0)
        {
            printf("timeout!\n"); // 监听超时
        }
        else if (poll_ret == -1)
        {
            perror("err!"); // 监听出错
        }

        // 正确监听到变化
        if (pollfds[0].revents & POLLIN || pollfds[0].revents & POLLPRI) // 如果新客户端连接
        {
            // 响应用户连接
            connect_fd[connect_cnt] = accept(listen_fd, NULL, NULL); // 返回 新的连接响应的套接字
            if (connect_fd[connect_cnt] == -1)
            {
                perror("accept");
                return -1;
            }
            memset(fd_ip, '\0', MAX_LINE);
            getfd_info(connect_fd[connect_cnt], fd_ip, &fd_port);
            printf("[Conn]: [%s:%d]\n", fd_ip, fd_port);

            // 更新客户端套接字集合
            pollfds[connect_cnt + 1].fd     = connect_fd[connect_cnt];
            pollfds[connect_cnt + 1].events = POLLIN | POLLPRI; // 任意优先级可读
            connect_cnt++;
        }
        else
        {
            int i = 0;
            // 如果是 客户端发生了数据可读)
            for (i = 0; !(pollfds[i + 1].revents & POLLIN); i++) {}

            // 4   接收与发送数据
            memset(buf, '\0', MAX_LINE);
            memset(fd_ip, '\0', MAX_LINE);
            getfd_info(connect_fd[i], fd_ip, &fd_port);
            ret = recv(connect_fd[i], buf, MAX_LINE, 0);
            if (ret == -1)
            {
                perror("recv");
                return -1;
            }

            if (errno == EINTR)
                continue;
            // 客户端断开
            if (ret == 0)
            {
                printf("[Off ]: [%s:%d]\n", fd_ip, fd_port);
                close(connect_fd[i]);

                connect_fd[i] = -1;
                memset(&pollfds[i + 1], 0, sizeof(struct pollfd)); // 清空断开套接字监听信息
                continue;
            }
            printf("[Recv]: [%s:%d] %s\n", fd_ip, fd_port, buf);
            send(connect_fd[i], buf, strlen(buf), 0);
        }
    }

    // 5 关闭套接字
    for (int i = 0; i < connect_cnt; i++)
    {
        close(connect_fd[i]);
    }
    close(listen_fd);

    return 0;
}
