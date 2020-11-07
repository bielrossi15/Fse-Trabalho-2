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
void * i2c();
void * get_sensor_values();
void * client_handler();
void * server_handler();
void * sensor_update();
void * temperature_handler();

double T = 0.0,
      H = 0.0,
      AC_TEMP = 100.0;

int lamp[4],
    ac[2],
    sp[2],
    so[6],
    sp_counter[2],
    count = 0,
    ac_on_mode = 0;

pthread_t t0, t1, t2, t3, t4, t5;
FILE *file;
sem_t sem;


int main(int argc, const char * argv[])
{
    sp_counter[0] = 0;
    sp_counter[1] = 0;

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
    
    pthread_create(&t0, NULL, server_handler, NULL);
    
    pthread_create(&t1, NULL, client_handler, NULL);

    sem_init(&sem, 0, 0);

    pthread_join(t0, NULL);

    return 0;
}

void sig_handler(int signal)
{
    printf("\nReceived signal %d, terminating program...\n", signal);
    alarm(0);
    pthread_cancel(t0);
    close_sockets();
    close_socket();
    set_lamp_state(0, 1);
    set_lamp_state(0, 2);
    set_lamp_state(0, 3);
    set_lamp_state(0, 4);
    set_ac_state(0, 1);
    set_ac_state(0, 2);
    bcm2835_close();
    exit(0);
}

void alarm_handler(int signal)
{
    sem_post(&sem);
    i2c();
    get_sensor_values();
    pthread_create(&t3, NULL, temperature_handler, NULL);
    alarm(1);
}   

void * i2c()
{
    initialize_bme("/dev/i2c-1", &T, &H);
}

void * get_sensor_values()
{
    get_state(lamp, ac, sp, so);
}

void * temperature_handler()
{
    if(T > AC_TEMP)
    {     
        if(ac_on_mode == 0)
        {
            set_ac_state(1, 3);
            ac_on_mode = 1;
            printf("%.2lf %.2lf\n", T, AC_TEMP);
        }
    }

    else
    {
        set_ac_state(0, 3);
    }
}

void * server_handler()
{
    connection_handler(&AC_TEMP);
}

void * client_handler()
{
    while(1)
    {
        sem_wait(&sem);
        if(message(&T, &H, lamp, ac, sp, so))
        {
            printf("Error sending data, trying again...\n");
        }
    }
}