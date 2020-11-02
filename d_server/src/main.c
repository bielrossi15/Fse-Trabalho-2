#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// including my own files
#include "bme280_i2c.h"
#include "gpio.h"
#include "server.h"
#include "client.h"

// functions to handle signals
void sig_handler(int signal);
void alarm_handler(int signal);

// functions to execute on threads
void i2c();
void get_sensor_values();
void print_val();
void * client_handler();

// helper functions
void format_time(char *date_string, char *hour_string);
void clear_outputs();

float T = 0.0,
      H = 0.0;

int sp[2],
    so[6];

pthread_t t0, t1;
FILE *file;
sem_t sem;


int main(int argc, const char * argv[])
{

    configure_pins();

    // all handled signals
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGALRM, alarm_handler);
    alarm(1);

    if(init_server() < 0)
    {
        fprintf(stderr, "Error creating server");
        exit(-1);
    }

    if(pthread_create(&t0, NULL, connection_handler, NULL))
    {
        exit(-2);
    }

    if(pthread_create(&t1, NULL, client_handler, NULL))
    {
        exit(-3);
    }

    sem_init(&sem, 0, 0);

    pthread_join(t0, NULL);

    return 0;
}

void sig_handler(int signal)
{
    printf("\nReceived signal %d, terminating program...\n", signal);
    alarm(0);
    pthread_cancel(t1);
    close_sockets();
    exit(0);
}

void alarm_handler(int signal)
{
    clear_outputs();
    i2c();
    get_sensor_values();
    //print_val();
    sem_post(&sem);
    alarm(1);
}   

void i2c()
{
    initialize_bme("/dev/i2c-1", &T, &H);
}

void get_sensor_values()
{
    get_sensor_state(sp, so);
} 

void * client_handler()
{
    while(1)
    {
        sem_wait(&sem);
        if(message(&T, &H, sp, so))
        {
            printf("Client failed sending data\n");
        }
    }
}

void print_val()
{
    printf("T -> %0.2f H -> %0.2f\n", T, H);
    
    for(int i = 0; i < *(&sp + 1) - sp; i++)
    {
        printf("SP_%d -> %d\n", i + 1, sp[i]);
    }

    for(int i = 0; i < *(&so + 1) - so; i++)
    {
        printf("SO_%d -> %d\n", i + 1, so[i]);
    }
}


void clear_outputs() {
    #if defined _WIN32
        system("cls");
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
    #elif defined (__APPLE__)
        system("clear");
    #endif
}