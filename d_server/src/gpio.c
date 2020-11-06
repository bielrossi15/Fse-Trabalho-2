#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <gpio.h>

#include "client.h"

#define L_1 RPI_V2_GPIO_P1_11 // KITCHEN
#define L_2 RPI_V2_GPIO_P1_12 // LIVING ROOM
#define L_3 RPI_V2_GPIO_P1_13 // BEDROOM 1
#define L_4 RPI_V2_GPIO_P1_15 // BEDROOM 2

#define AC_1 RPI_V2_GPIO_P1_16 // BEDROOM 1
#define AC_2 RPI_V2_GPIO_P1_18 // BEDROOM 2

#define SP_1 RPI_V2_GPIO_P1_22 // LIVING ROOM
#define SP_2 RPI_V2_GPIO_P1_37 // KITCHEN

#define SO_1 RPI_V2_GPIO_P1_29 // KITCHEN DOOR
#define SO_2 RPI_V2_GPIO_P1_31 // KITCHEN WINDOW
#define SO_3 RPI_V2_GPIO_P1_32 // LIVING ROOM DOOR
#define SO_4 RPI_V2_GPIO_P1_36 // LIVING ROOM WINDOW
#define SO_5 RPI_V2_GPIO_P1_38 // BEDROOM 1 WINDOW
#define SO_6 RPI_V2_GPIO_P1_40 // BEDROOM 2 WINDOW

void configure_pins()
{
    if(!bcm2835_init())
    {
        fprintf(stderr, "Error initializing bcm2835\n");
        return;
    }

    // output pins
    bcm2835_gpio_fsel(L_1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(L_2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(L_3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(L_4, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_fsel(AC_1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(AC_2, BCM2835_GPIO_FSEL_OUTP);

    // input pins
    bcm2835_gpio_fsel(SP_1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SP_2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SO_1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SO_2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SO_3, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SO_4, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SO_5, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SO_6, BCM2835_GPIO_FSEL_INPT);

}

void set_lamp_state(int value, int opt)
{
    if(value != 0 && value != 1)
    {
        return;
    }

    switch(opt)
    {
        case 1:
            bcm2835_gpio_write(L_1, value);
            break;
        case 2:
            bcm2835_gpio_write(L_2, value);
            break;
        case 3:
            bcm2835_gpio_write(L_3, value);
            break; 
        case 4:
            bcm2835_gpio_write(L_4, value);
            break;
        default:
            break;
    }
}

void set_ac_state(int value, int opt)
{
    if(value != 0 && value != 1)
    {
        return;
    }

    switch(opt)
    {
        case 1:
            bcm2835_gpio_write(AC_1, value);
            break;
        case 2:
            bcm2835_gpio_write(AC_2, value);
            break;
        case 3:
            bcm2835_gpio_write(AC_1, value);
            bcm2835_gpio_write(AC_2, value);
            break;
        default:
            break;
    }
}

void get_sensor_state(int lamp[], int ac[], int sp[], int so[])
{
    lamp[0] = bcm2835_gpio_lev(L_1);
    lamp[1] = bcm2835_gpio_lev(L_2);
    lamp[2] = bcm2835_gpio_lev(L_3);
    lamp[3] = bcm2835_gpio_lev(L_4);

    ac[0] = bcm2835_gpio_lev(AC_1);
    ac[1] = bcm2835_gpio_lev(AC_2);
    
    sp[0] = bcm2835_gpio_lev(SP_1);
    sp[1] = bcm2835_gpio_lev(SP_2);

    so[0] = bcm2835_gpio_lev(SO_1);
    so[1] = bcm2835_gpio_lev(SO_2);
    so[2] = bcm2835_gpio_lev(SO_3);
    so[3] = bcm2835_gpio_lev(SO_4);
    so[4] = bcm2835_gpio_lev(SO_5);
    so[5] = bcm2835_gpio_lev(SO_6);
}
