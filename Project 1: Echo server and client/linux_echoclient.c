// linux echo client
// build using gcc linux_echoclient.c -o [executable name] and run using ./[executable name] [ip address]
// refrences used: https://github.com/saul-h/file-server/blob/master/tcp_ip_client.c & https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

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

    // the server will send a message to the client saying it connected and waits with a delay(2000) before it start listening for a message from the client, so we need to wait for the server to start listening before we send a message.
    sleep(2);
    
    // send a message to the server
    char message[MAXBUF] = "client was able to connect to server";
    char response[MAXBUF];
    send(sock, message, strlen(message), 0);
    
    // receive a response and output in the terminal
    recv(sock, response, MAXBUF, 0);
    printf("Client received message: %s\n", response);

    // close the connection
    shutdown(sock, SHUT_RDWR);

    return 0;

}