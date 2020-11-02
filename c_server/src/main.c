#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// including my own files
#include "server.h"


// functions to handle signals
void sig_handler(int signal);
void alarm_handler(int signal);

// functions to execute on threads
void * write_file();
void * server();

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

    init_server();

    // all handled signals
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGALRM, alarm_handler);
    alarm(1);

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
    alarm(1);
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