#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_LINE  80
#define SERV_PORT 6666

int main(void)
{
    struct sockaddr_in ser_addr;
    char               buf_send[MAX_LINE];
    char               buf_recv[MAX_LINE];
    int                socket_fd;

    // 1. 创建通信套接字
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 设置连接地址，并连接
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family      = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_addr.sin_port        = htons(SERV_PORT);

    if (-1 == connect(socket_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)))
    {
        printf("Connect failed");
        close(socket_fd);
        return -1;
    }

    while (1)
    {
        memset(buf_send, '\0', MAX_LINE);
        memset(buf_recv, '\0', MAX_LINE);

        printf("[Send] %s", buf_send);
        fgets(buf_send, MAX_LINE, stdin);
        buf_send[strlen(buf_send) - 1] = '\0';
        if (!strcmp(buf_send, "bye"))
        {
            break;
        }

        if (strlen(buf_send))
        {
            // 3. 发送信息
            write(socket_fd, buf_send, strlen(buf_send));

            // 4. 接收信息
            int len = read(socket_fd, buf_recv, MAX_LINE);
            printf("[Recv] %s\n", buf_recv);
        }
    }

    // 5. 关闭连接
    close(socket_fd);
    return 0;
}
