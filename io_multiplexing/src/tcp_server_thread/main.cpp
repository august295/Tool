#include <iostream>
#include <stdio.h>
#include <string.h>
#include <thread>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define SER_PORT 6666
#define MAX_LINE 1024

void thread_accept(int client_fd)
{
    char buf[MAX_LINE] = {0}; // ���ݳ���

    // ѭ������
    while (true)
    {
        // ��ջ�����
        memset(buf, '\0', MAX_LINE);
        // ������Ϣ
        if (recv(client_fd, buf, MAX_LINE, 0) > 0)
        {
            std::cout << "[" << std::this_thread::get_id() << "] [Recv]: " << buf << std::endl;
            send(client_fd, buf, strlen(buf), 0);
            if (strcmp(buf, "quit") == 0)
            {
                break;
            }
        }
    }
    close(client_fd);
}

int main(int argc, char* argv[])
{
    int listen_fd = 0; // ����������
    int ret       = 0; // ����ֵ

    // �����׽���
    listen_fd = socket(AF_INET, SOCK_STREAM, 0); // IPV4, TCP socket
    if (listen_fd == -1)
    {
        printf("Socket create failed\n");
        return -1;
    }

    // ָ����ַ
    struct sockaddr_in ser_addr;
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family      = AF_INET;           // ��ַЭ����
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ָ�� IP ��ַ inet_addr("127.0.0.1")
    ser_addr.sin_port        = htons(SER_PORT);   // ָ���˿ں�

    // ��������
    bind(listen_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));

    // ����
    ret = listen(listen_fd, 10);
    if (ret == -1)
    {
        printf("Listen create failed.\n");
        return -1;
    }
    printf("Listening...\n");

    // ѭ���������ӣ�����ǰ��Ȼ���������Ӻ���̷߳��봦��
    while (true)
    {
        struct sockaddr_in client_addr;
        int                client_addr_len = sizeof(client_addr);
        int                client_fd       = accept(listen_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
        if (client_fd == -1)
        {
            printf("Accept failed.\n");
            return -1;
        }
        printf("[%s:%d] connect\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
        std::thread t(thread_accept, client_fd);
        t.detach();
    }

    // �ر�����
    close(listen_fd);

    return 0;
}