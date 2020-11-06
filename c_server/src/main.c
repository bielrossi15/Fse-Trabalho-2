#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// including my own files
#include "server.h"
#include "client.h"

#define HOUR_SIZE 9
#define DATE_SIZE 11

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
void file_write(int type, int number, int on_off, double temp);

double T = 0.0,
      H = 0.0;

int lamp[4],
    ac[2],
    sp[2],
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
    connection_handler(&H, &T, lamp, ac, sp, so);
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

void format_time(char *date_string, char *hour_string) 
{
    time_t rawtime;
    struct tm * tm_data;

    time(&rawtime);
    tm_data = localtime(&rawtime);

    sprintf(hour_string, "%02d:%02d:%02d", tm_data->tm_hour, tm_data->tm_min, tm_data->tm_sec);

    sprintf(date_string, "%02d-%02d-%04d", tm_data->tm_mday, tm_data->tm_mon+1, 1900+tm_data->tm_year);
}

void file_write(int type, int number, int on_off, double temp)
{
    char date[DATE_SIZE];
    char hour[HOUR_SIZE];

    
    format_time(date, hour);

    file = fopen("./data.csv", "a+");

    if(type == 0)
    {
        if(on_off == 1)
            fprintf(file, "%s %s -> turned on lamp %d\n", date, hour, number);
        else
            fprintf(file, "%s %s -> turned off lamp %d\n", date, hour, number);
    }

    else if(type == 1)
    {
        if(on_off == 1)
            fprintf(file, "%s %s -> turned on ac %d\n", date, hour, number);
        else
            fprintf(file, "%s %s -> turned off ac %d\n", date, hour, number);
    }

    else if(type == 2)
    {
        fprintf(file, "%s %s -> changed ac temperature to %.2lf\n", date, hour, temp);
    }
    
    fclose(file);

}