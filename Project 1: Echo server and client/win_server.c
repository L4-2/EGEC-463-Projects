// echo server for windows

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 256

#define PORT 5500

int main()
{

    // initializing winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // creating socket
    SOCKET serversocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serversocket == INVALID_SOCKET)
    {
        printf("socket() error\n");
        exit(1);
    }

    // binding socket to port
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serversocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
    {
        printf("bind() error\n");
        exit(1);
    }

    // listening
    if (listen(serversocket, 5) == SOCKET_ERROR)
    {
        WSACleanup();
        printf("listen() error\n");
        exit(1);
    }

    printf("waiting for client...\n");

    // accepting
    SOCKET clientsocket;
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);

    // receiving and sending
    char buf[BUF_SIZE];

    clientsocket = accept(serversocket, (struct sockaddr *)&clientaddr, &addrlen);
    if (clientsocket == INVALID_SOCKET)
    {
        printf("accept() error\n");
        exit(1);
    }
    printf("connected client: %s\n", inet_ntoa(clientaddr.sin_addr));
    int len = recv(clientsocket, buf, sizeof(buf), 0);
    if (len == SOCKET_ERROR)
    {
        printf("recv() error\n");
        exit(1);
    }
    buf[len] = '\0';
    printf("received data: %s\n", buf);
    if (send(clientsocket, buf, strlen(buf), 0) == SOCKET_ERROR)
    {
        printf("send() error\n");
        exit(1);
    }

    // closing socket
    closesocket(clientsocket);
    closesocket(serversocket);
    
}
