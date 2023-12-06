// linux echo client
// build using gcc linux_echoclient.c -o [executable name] and run using ./[executable name] [ip address]
// refrences used: https://github.com/saul-h/file-server/blob/master/tcp_ip_client.c & https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAXBUF 256

#define PORT 80

char IP[] = "192.168.137.243";

char temp_unit;

struct subscribe_params
{
    int x;
    int y;
};

// function to convert raw ADC value to temperature
float convert_temp(int adc)
{
    float temp = 0;
    if (temp_unit == 'f')
    {
        temp = (((adc - 500) / 10) * 1.8) + 32;
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
int readADC()
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

// The thread function
void *subscribe_thread(void *arg)
{
    struct subscribe_params *params = (struct subscribe_params *)arg;
    for (int i = 0; i < params->x; i++)
    {
        readADC();
        sleep(params->y);
    }
    free(arg);
    return NULL;
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
        // create a thread to send readADC() every y seconds
        pthread_t thread_id;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        struct subscribe_params *params = malloc(sizeof(struct subscribe_params));
        params->x = x;
        params->y = y;
        pthread_create(&thread_id, &attr, subscribe_thread, params);

        pthread_attr_destroy(&attr);
    }
}

int main()
{

    printf("Welcome to the temperature sensor client.\n");
    printf("Please enter a command (toggle, read, subscribe, set_temp_unit, exit)\n");

    char command[MAXBUF];

    // loop until the user enters exit

    while (1)
    {
        printf(">> ");
        scanf("%s", command);

        // if the user enters toggle, ask for a color and toggle the led
        if (strcmp(command, "toggle") == 0)
        {
            char color[MAXBUF];
            printf("Please enter a color (red or white)\n");
            scanf("%s", color);
            toggle_led(color);
        }
        // if the user enters read, read the temperature
        else if (strcmp(command, "read") == 0)
        {
            readADC();
        }
        // if the user enters subscribe, ask for the amount of readings and the interval between readings
        else if (strcmp(command, "subscribe") == 0)
        {
            int x, y;
            printf("Please enter the amount of readings you want to get.\n");
            scanf("%d", &x);
            printf("Please enter the interval between readings.\n");
            scanf("%d", &y);
            subscribe(x, y);
        }
        // if the user enters set_temp_unit, ask for a unit and set the unit
        else if (strcmp(command, "set_temp_unit") == 0)
        {
            char unit;
            printf("Please enter a unit (f or c)\n");
            scanf("%s", &unit);
            set_temp_unit(unit);
        }
        // if the user enters exit, exit the program
        else if (strcmp(command, "exit") == 0)
        {
            printf("Exiting program.\n");
            exit(0);
        }
        // if the user enters an invalid command, print an error message
        else
        {
            printf("Please enter a valid command.\n");
        }
    }

    return 0;
}

// this is for reference only, this is the code for the client that connects to the server and sends a message.

//  // check if user passed an ip address.
//     if (argc != 2)
//     {
//         printf("Please enter a valid ip address.\n");
//         exit(1);
//     }

//     // create the socket
//     int sock = socket(AF_INET, SOCK_STREAM, 0);
//     // if an error occurs, exit the program.
//     if (sock < 0)
//     {
//         printf("Error creating socket.\n");
//         exit(1);
//     }

//     // create the server struct and connect to server
//     struct sockaddr_in server;
//     server.sin_family = AF_INET;
//     server.sin_port = htons(PORT);
//     server.sin_addr.s_addr = inet_addr(argv[1]);

//     // send a connection request to the server
//     if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
//     {
//         printf("Error connecting to server.\n");
//         exit(1);
//     }
//     else
//     {
//         printf("Client connected to server.\n");

//         // the server will send a message to the client saying it connected and waits with a delay(2000) before it start listening for a message from the client, so we need to wait for the server to start listening before we send a message.
//         sleep(2);

//         // send a message to the server
//         char message[MAXBUF] = "client was able to connect to server";
//         char response[MAXBUF];
//         send(sock, message, strlen(message), 0);

//         // receive a response and output in the terminal
//         recv(sock, response, MAXBUF, 0);
//         printf("Client received message: %s\n", response);

//         // close the connection
//         shutdown(sock, SHUT_RDWR);
//     }