#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// including my own files
#include "bme280_i2c.h"
#include "gpio.h"



#define HOUR_SIZE 9
#define DATE_SIZE 11

// functions to handle signals
void sig_handler(int signal);
void alarm_handler(int signal);

// functions to execute on threads
void i2c();
void get_sensor_values();
void print_val();
void * menu();

// helper functions
void format_time(char *date_string, char *hour_string);
void clear_outputs();

float T = 0.0,
      H = 0.0;
int sp[2],
    so[6];

pthread_t t0, t1;
FILE *file;

char str_TR[50] = "",
     str_HIST[50] = "";


int main(int argc, const char * argv[])
{

    configure_pins();

    // all handled signals
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGALRM, alarm_handler);
    alarm(1);

    while(1)
    {
        pause();
    }
    return 0;
}

void sig_handler(int signal)
{
    printf("\nReceived signal %d, terminating program...\n", signal);
    alarm(0);
    exit(0);
}

void alarm_handler(int signal)
{
    clear_outputs();
    i2c();
    get_sensor_values();
    print_val();
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

void format_time(char *date_string, char *hour_string) 
{
    time_t rawtime;
    struct tm * tm_data;

    time(&rawtime);
    tm_data = localtime(&rawtime);

    sprintf(hour_string, "%02d:%02d:%02d", tm_data->tm_hour, tm_data->tm_min, tm_data->tm_sec);

    sprintf(date_string, "%02d-%02d-%04d", tm_data->tm_mday, tm_data->tm_mon+1, 1900+tm_data->tm_year);
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