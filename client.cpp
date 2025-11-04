#include "client.h"
#pragma comment(lib, "Ws2_32.lib")
#define BUFF_SIZE 1024
char buff[BUFF_SIZE];
std::mutex mtx;

// 循环发送消息
unsigned sendMsg(void* arg)
{
    SOCKET sock = *((SOCKET*)arg);

    while (1)
    {
        // 输入时也加锁，避免提示符错乱
        mtx.lock();
        printf("");  // 确保光标在正确位置
        mtx.unlock();

        scanf("%s", buff);

        if (strcmp(buff, "quit") == 0)
        {
            closesocket(sock);
            exit(0);
        }

        send(sock, buff, strlen(buff), 0);
    }
    return 0;
}

// 循环接收消息
unsigned recvMsg(void* arg)
{
    SOCKET sock = *((SOCKET*)arg);
    char msg[BUFF_SIZE];

    while (1)
    {
        int msg_len = recv(sock, msg, sizeof(msg) - 1, 0);
        if (msg_len <= 0) return -1;

        msg[msg_len] = '\0';

        // 加锁保护输出
        mtx.lock();
        printf("\n%s\n", msg);  // 换行+消息+换行+提示符
        fflush(stdout);
        mtx.unlock();
    }
    return 0;
}

int main()
{
    // 初始化winsock
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return -1;
    }

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return -1;
    }

    // 创建socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN servaddr;

    // bind绑定socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8086); // 云服务器开放端口
    inet_pton(AF_INET, "8.155.39.50", &servaddr.sin_addr); // 此处为云服务器ip，可以修改

    // 连接服务端
    if (connect(sockfd, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
    {
        printf("connect error: %d", GetLastError());
        exit(1);
    }
    else
    {
        printf("欢迎来到聊天室，请输入用户名: ");
    }

    // 创建循环发送和接收线程
    std::thread hSendHand(sendMsg, (void*)&sockfd);
    std::thread hRecvHand(recvMsg, (void*)&sockfd);

    hSendHand.join();
    hRecvHand.join();

    closesocket(sockfd);
    WSACleanup();

    return 0;
}