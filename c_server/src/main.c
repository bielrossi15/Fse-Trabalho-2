#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

// including my own files
#include "server.h"
#include "client.h"
#include "helper_functions.h"

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

double T = 0.0,
       H = 0.0;

int lamp[4],
    ac[2],
    sp[2],
    so[6],
    sp_old[2],
    so_old[6];

pthread_t t0, t1;

int main(int argc, const char * argv[])
{

    memset(sp_old, 1, 2 * sizeof(sp_old[0]));
    memset(so_old, 1, 6 * sizeof(so_old[0]));
    
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
    connection_handler(&H, &T, lamp, ac, sp, so);
}

void * sensor_status()
{
    for(int i = 0; i < 2; i++)
    {
        if(sp[i] != sp_old[i] && sp[i] == 1)
        {
            file_write(3, 0, 0, 0.0);
        }
        sp_old[i] = sp[i];
    }

    for(int i = 0; i < 6; i++)
    {
        if(so[i] != so_old[i] && so[i] == 1)
        {
            file_write(3, 0, 0, 0.0);
        }

        so_old[i] = so[i];
    }

    
}

void sig_handler(int signal)
{
    printf("\nReceived signal %d, terminating program...\n", signal);
    alarm(0);
    close_sockets();
    exit(0);
}

void alarm_handler(int signal)
{

    if(pthread_create(&t0, NULL, server_handler, NULL))
    {
        exit(-2);
    }

    if(pthread_create(&t1, NULL, sensor_status, NULL))
    {
        exit(-2);
    }

    alarm(1);
}

void menu()
{
    
    short cs = 0;

    int l, a, l_on_off, ac_on_off;
    double temp;

    clear_outputs();

    printf("========== INTERACTIVE MENU ==========\n  1 -> Print values\n  2 -> Turn on/off lamps\n  3 -> Turn on/off air c\n  4 -> Choose new AC temperature\n ======================================\n");
    printf("-> ");
    scanf("%hd", &cs);
    printf("\n");
    switch (cs)
    {
        case 1:
            printf("T -> %lf\nH -> %lf\n", H, T);
            
            for(int i = 0; i < *(&lamp + 1) - lamp; i++)
                printf("LAMP_%d -> %d\n", i+1, lamp[i]);

            for(int i = 0; i < *(&ac + 1) - ac; i++)
                printf("AC_%d -> %d\n", i+1, ac[i]);

            for(int i = 0; i < *(&sp + 1) - sp; i++)
                printf("SP_%d -> %d\n", i+1, sp[i]);
            
            for(int i = 0; i < *(&so + 1) - so; i++)
                printf("SO_%d -> %d\n", i+1, so[i]);

            char q;
            printf("PRESS 'q' TO RETURN TO MENU");
        
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
            printf("Wich lamp you like to control? (1-4)\n");
            scanf("%d", &l);

            printf("\n0 - OFF | 1 - ON\n");
            scanf("%d", &l_on_off);


            if(message(0, l, l_on_off, 0.0))
                printf("Error sending request\n");

            char j;
            printf("PRESS 'q' TO RETURN TO MENU");

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
            file_write(0, l, l_on_off, 0.0);
        break;

        case 3:
            printf("Wich AC you like to control? (1-2)\n");
            scanf("%d", &a);

            printf("\n0 - OFF | 1 - ON\n");
            scanf("%d", &ac_on_off);


            if(message(1, a, ac_on_off, 0.0))
                printf("Error sending request\n");

            char c;
            printf("PRESS 'q' TO RETURN TO MENU");

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
            file_write(1, a, ac_on_off, 0.0);
        break;

        case 4:
            printf("Choose an AC temperature\n");
            scanf("%lf", &temp);

            if(message(2, 0, 0, temp))
                printf("Error sending request\n");

            char n;
            printf("PRESS 'q' TO RETURN TO MENU");

            while(1)
            {
                scanf("%c", &n);
                printf("\n");

                if(n == 'q')
                {
                    clear_outputs();
                    break;
                }
            }
            file_write(2, 1, 0, temp);
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