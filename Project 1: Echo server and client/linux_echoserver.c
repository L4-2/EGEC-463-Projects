// linux echo client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXBUF 256

#define PORT 80

int main(int argc, char *argv[]){

    // check if user passed an ip address.
    if(argc != 2){
        printf("Please enter a valid ip address.\n");
        exit(1);
    }
    
    // create the socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
        // if an error occurs, exit the program.
    if(sock < 0){
        printf("Error creating socket.\n");
        exit(1);
    }

    // create the server struct and connect to server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    // send a connection request to the server
    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        printf("Error connecting to server.\n");
        exit(1);
    }else{
        printf("Client connected to server.\n");
    }

    // send a message to the server and receive a response and output in the terminal
    char message[MAXBUF] = "client was able to connect to server";
    char response[MAXBUF];
    while(1){
        send(sock, message, strlen(message), 0);
        recv(sock, response, MAXBUF, 0);
        printf("Client received message: %s\n", response);
    }

    return 0;

}