// linux echo server
// build using gcc linux_echoserver.c -o [executable name] and run using ./[executable name]
// refrences used: https://mohsensy.github.io/programming/2019/09/25/echo-server-and-client-using-sockets-in-c.html & https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 256

#define PORT 80

// create an echo function that will be used to send a message back to the client
void echo(int client_sock){
    char buffer[MAXBUF];
    int message_size = 0;
    // receive a message from the client
    message_size = recv(client_sock, buffer, MAXBUF, 0);
    // send the message back to the client
    send(client_sock, buffer, message_size, 0);
}

int main(int argc, char const *argv[]){

    // create the socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        printf("Error creating socket.\n");
        exit(1);
    }

    // bind socket to port
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        printf("Error binding socket to port.\n");
        exit(1);
    }

    // listen for a connection
    listen(sock, 1);
    printf("Server listening for a connection.\n");

    // accept a connection
    int client_sock;
    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);
    while ((client_sock = accept(sock, (struct sockaddr *)&client, &client_size)) > 0){
        printf("Server accepted a connection.\n");
        // Echo the message back to the client
        echo(client_sock);
        // close the connection
        shutdown(client_sock, SHUT_RDWR);
    }

}
