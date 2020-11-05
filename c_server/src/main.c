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
void * server_handler();
void menu();

// helper functions
void format_time(char *date_string, char *hour_string);
void clear_outputs();

float T = 0.0,
      H = 0.0;

int sp[2],
    so[6];

pthread_t t0, t1;
FILE *file;

int main(int argc, const char * argv[])
{

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

    while(1)
    {
        menu();
    }

    return 0;
}

void * server_handler()
{
    connection_handler(&H, &T, sp, so);
}

void sig_handler(int signal)
{
    printf("\nReceived signal %d, terminating program...\n", signal);
    alarm(0);
    exit(0);
}

void alarm_handler(int signal)
{
    if(pthread_create(&t0, NULL, server_handler, NULL))
    {
        exit(-2);
    }

    alarm(1);
}

void menu()
{
    
    short cs = 0;

    clear_outputs();

    printf("========== INTERACTIVE MENU ==========\n  1 -> Print values\n  2 -> Turn on/off lamps\n  3 -> Turn on/off air c\n\n======================================\n");
    printf("-> ");
    scanf("%hd", &cs);
    printf("\n");
    switch (cs)
    {
        case 1:
            printf("T -> %0.2f\nH -> %0.2f\n", H, T);
            
            for(int i = 0; i < *(&sp + 1) - sp; i++)
                printf("SP_%d -> %d\n", i, sp[i]);
            
            for(int i = 0; i < *(&so + 1) - so; i++)
                printf("SO_%d -> %d\n", i, so[i]);

            char q;
            printf("\nPRESS 'q' TO RETURN TO MENU");
        
            while(1)
            {
                scanf("%c", &q);
                printf("\n");

                if(q == 'q')
                {
                    clear_outputs();
                    break;
                }
            }

        break;

        case 2:
            printf("lamp on\n");
            char j;
            printf("\nPRESS 'q' TO RETURN TO MENU");

            while(1)
            {
                scanf("%c", &j);
                printf("\n");

                if(j == 'q')
                {
                    clear_outputs();
                    break;
                }
            }
        break;

        case 3:
            printf("air c on\n");
            char c;
            printf("\nPRESS 'q' TO RETURN TO MENU");

            while(1)
            {
                scanf("%c", &c);
                printf("\n");

                if(c == 'q')
                {
                    clear_outputs();
                    break;
                }
            }
        break;

        default:
        break;
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