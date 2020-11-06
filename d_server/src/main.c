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
void * sensor_update();

float T = 0.0,
      H = 0.0;

int lamp[4],
    ac[2],
    sp[2],
    so[6],
    count = 0;

pthread_t t0, t1, t2, t3, t4, t5;
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
    ualarm(200000, 200000);

    if(init_server() < 0)
    {
        fprintf(stderr, "Error creating server");
        exit(-1);
    }
    
    pthread_create(&t0, NULL, connection_handler, NULL);
    
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
    exit(0);
}

void alarm_handler(int signal)
{
    pthread_create(&t2, NULL, sensor_update, NULL);

    if(count == 5)
    {
        sem_post(&sem);
        i2c();
        get_sensor_values();
        count = 0;
    }

    count++;
}   

void * i2c()
{
    initialize_bme("/dev/i2c-1", &T, &H);
}

void * get_sensor_values()
{
    get_sensor_state(lamp, ac, sp, so);
}

void * sensor_update()
{
    int a[4],
        b[2],
        sp_a[2],
        so_a[6];
    
    char sensor_data[2];
    
    get_sensor_state(a, b, sp_a, so_a);

    for(int i = 0; i < 2; i++)
    {
        if(sp_a[i] != sp[i] && sp_a[i] == 1)
        {
            sensor_data[0] = 'p';
            sensor_data[1] = i;
            sensor_message(sensor_data);
        }
            
    }

    for(int i = 0; i < 6; i++)
    {
        if(so_a[i] != so[i] && so_a[i] == 1)
        {
            sensor_data[0] = 'o';
            sensor_data[1] = i;
            sensor_message(sensor_data);
        }
    }    

}

void * client_handler()
{
    while(1)
    {
        sem_wait(&sem);
        if(message(&T, &H, lamp, ac, sp, so))
        {
            printf("Client failed sending data\n");
        }
    }
}