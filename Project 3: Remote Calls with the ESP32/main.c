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

char IP[] = "192.168.137.243";

char temp_unit;

// function to convert raw ADC value to temperature
float convert_temp(int adc)
{
    float temp = 0;
    if (temp_unit == 'f')
    {
        temp = (((adc- 500) / 10) * 1.8) + 32;
    }
    else if (temp_unit == 'c')
    {
        temp = (adc - 500) / 10;
    }
    else
    {
        printf("Please set a temperature unit (f or c)\n");
    }
    return temp;
} 

// subroutine for toggling specified led on/off
void toggle_led(char color[])
{

    // create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // if an error occurs, exit the program.
    if (sock < 0)
    {
        printf("Error creating socket.\n");
        exit(1);
    }

    // create the server struct and connect to server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP);

    // send a connection request to the server and send the command to toggle the led
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        // error handling
        printf("Error connecting to server.\n");
        exit(1);
    }
    else
    {

        // send 'toggle <color>' to the server
        char message[MAXBUF] = "toggle ";
        strcat(message, color);
        send(sock, message, strlen(message), 0);

        // disconnect from the server
        shutdown(sock, SHUT_RDWR);
    }
}

// subroutine for specifying fahrenheit or celsius unit for readings
void set_temp_unit(char unit)
{
    if (unit == 'f' || unit == 'c')
    {
        temp_unit = unit;
    }
    else
    {
        printf("Please enter a valid unit (f or c)\n");
    }
}

// subroutine for getting the temperature one time
void read()
{
    // create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // if an error occurs, exit the program.
    if (sock < 0)
    {
        printf("Error creating socket.\n");
        exit(1);
    }

    // create the server struct and connect to server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP);

    // send a connection request to the server and send the command to toggle the led
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        // error handling
        printf("Error connecting to server.\n");
        exit(1);
    }
    else
    {

        // send 'read' to the server
        char message[MAXBUF] = "read";
        send(sock, message, strlen(message), 0);

        // receive a response and output in the terminal
        char response[MAXBUF];
        recv(sock, response, MAXBUF, 0);
        printf("ADC value: %s\n", response);

        // convert the response to a float and print the temperature
        float ADC = convert_temp(atoi(response));

        // use the convert_temp function to convert the ADC to temperature and print it
        printf("Temperature: %f\n", ADC);

        // disconnect from the server
        shutdown(sock, SHUT_RDWR);
    }
}

// subroutine for getting x amount of temperature readings in y second intervals
void subscribe(int x, int y)
{
    // create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // if an error occurs, exit the program.
    if (sock < 0)
    {
        printf("Error creating socket.\n");
        exit(1);
    }

    // create the server struct and connect to server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP);

    // send a connection request to the server and send the command to toggle the led
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        // error handling
        printf("Error connecting to server.\n");
        exit(1);
    }
    else
    {
        // 
    }
}

int main()
{

    return 0;
}

// this is for reference only, this is the code for the client that connects to the server and sends a message.

 // check if user passed an ip address.
    if (argc != 2)
    {
        printf("Please enter a valid ip address.\n");
        exit(1);
    }

    // create the socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // if an error occurs, exit the program.
    if (sock < 0)
    {
        printf("Error creating socket.\n");
        exit(1);
    }

    // create the server struct and connect to server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    // send a connection request to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Error connecting to server.\n");
        exit(1);
    }
    else
    {
        printf("Client connected to server.\n");

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
    }
