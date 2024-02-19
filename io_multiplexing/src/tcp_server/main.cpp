#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define SER_PORT 6666
#define MAX_LINE 1024

int main(int argc, char* argv[])
{
    int  listen_fd     = 0;   // 监听描述符
    int  ret           = 0;   // 返回值
    char buf[MAX_LINE] = {0}; // 数据长度

    // 创建套接字
    listen_fd = socket(AF_INET, SOCK_STREAM, 0); // IPV4, TCP socket
    if (listen_fd == -1)
    {
        printf("Socket create failed\n");
        return -1;
    }

    // 指定地址
    struct sockaddr_in ser_addr;
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family      = AF_INET;           // 地址协议族
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定 IP 地址 inet_addr("127.0.0.1")
    ser_addr.sin_port        = htons(SER_PORT);   // 指定端口号

    // 服务器绑定
    bind(listen_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));

    // 监听
    ret = listen(listen_fd, 10);
    if (ret == -1)
    {
        printf("Listen create failed.\n");
        return -1;
    }
    printf("Listening...\n");

    // 建立连接
    struct sockaddr_in client_addr;
    int                client_addr_len = sizeof(client_addr);
    int                client_fd       = accept(listen_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
    if (client_fd == -1)
    {
        printf("Accept failed.\n");
        return -1;
    }
    printf("[%s:%d] connect\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

    // 循环接收
    while (true)
    {
        // 清空缓存区
        memset(buf, '\0', MAX_LINE);
        // 接收信息
        if (recv(client_fd, buf, MAX_LINE, 0) > 0)
        {
            printf("[Recv]: %s\n", buf);
            send(client_fd, buf, strlen(buf), 0);
            if (strcmp(buf, "quit") == 0)
            {
                break;
            }
        }
    }

    // 关闭连接
    close(client_fd);
    close(listen_fd);

    return 0;
}