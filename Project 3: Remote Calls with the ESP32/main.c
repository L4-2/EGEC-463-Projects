// Lingesh Kumar
// linux ESP32 client
// build using gcc [filename].c -o [executable name] and run using ./[executable name]
// refrences used: https://www.youtube.com/watch?v=It0OFCbbTJE, https://www.geeksforgeeks.org/thread-functions-in-c-c/, https://stackoverflow.com/questions/2156353/how-do-you-query-a-pthread-to-see-if-it-is-still-running, my father

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAXBUF 256

#define PORT 80

// set the IP address of the server
char IP[] = "";

// set the default temperature unit to fahrenheit
char temp_unit = 'f';

// global variable to keep track of the number of threads running
int thread_count = 0;

// struct for passing parameters to the thread
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
        // convert the ADC to fahrenheit using the provided transfer function
        temp = (((adc - 500) / 10) * 1.8) + 32;
    }
    else if (temp_unit == 'c')
    {
        // temperature transfer function for celsius; provided by the manufacturer
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

        // use the convert_temp function to convert the ADC to temperature and print it and put unit using the temp_unit variable
        printf("Temperature: %.2f%c\n", ADC, temp_unit);

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
    thread_count--;
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
        // only create a thread if it is the only thread running
        if (thread_count == 0)
        {
            thread_count++;

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
        else
        {
            printf("Only one thread can run at a time.\n");
            return;
        }
    }
}

int main()
{

    printf("ESP32 temperature sensor client.\n");
    printf("Please enter a command (toggle, read, subscribe, set_u, exit)\n");

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
            printf("Please enter a color (green or red)\n");
            scanf("%s", color);
            toggle_led(color);
        }
        // if the user enters read, read the temperature
        else if (strcmp(command, "read") == 0)
        {
            readADC();
        }
        // if the user enters subscribe, ask for the amount of readings and the interval between readings and subscribe
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
        else if (strcmp(command, "set_u") == 0)
        {
            char unit;
            printf("Please enter a unit (f or c)\n");
            scanf("%s", &unit);
            set_temp_unit(unit);
        }
        // if the user enters exit, exit the program
        else if (strcmp(command, "exit") == 0)
        {
            // clean all the threads just in case
            pthread_exit(NULL);

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
