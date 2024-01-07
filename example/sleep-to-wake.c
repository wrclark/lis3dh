#define _GNU_SOURCE
#include <unistd.h> /* usleep() */
#include <stdio.h>
#include "lis3dh.h"
#include "interrupt.h"
#include "i2c.h"

/* LIS3DH INT2 connected to Raspberry Pi GPIO 16 (Pin 36) */
#define GPIO_INTERRUPT_PIN_INT2 16

int main() {
    
    lis3dh_t lis;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    /* initialise LIS3DH struct */
    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* reset device just in case */
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    /* register interrupt on specified pin */
    if (int_register(GPIO_INTERRUPT_PIN_INT2)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;

    /* enable ACT function through INT PIN 2 */
    /* interrupts must be used, there is no reg to poll */
    lis.cfg.pin2.act = 1;

    /* This functionality cannot be filtered internally !! */

    /* 1 LSb = 16 mg @ FS_2G 
     * 1 LSb = 32 mg @ FS_4G
     * 1 LSb = 62 mg @ FS_8G
     * 1 LSb = 186 mg @ FS_16G 
     * 
     * @ FS_2G
     * Wake up when acceleration goes above 1.3g .. (any axis)
     * 1.3g / 16mg = 1300/16 = N LSb = 81.25 ~ 81
     */

    /* This controls activity recognition */
    lis.cfg.act_ths = 81;   /* 7-bit ; must be > 1*/

    /* Sleep-to-wake and return-to-sleep duration 
     * 1 LSb = (8 * 1[LSb] + 1) / ODR
     * Confusing formulation !
     * 
     * [ODR] [1 LSb in milliseconds]
     *   1      1000
     *   10     100
     *   25     40
     *   50     20
     *   100    10
     *   200    5
     *   400    2.5
     * 
     *  Full table in lis3dh.h
     * 
     * 1 LSb = (8 * N[2.5ms] + 1) / ODR       ( @ ODR=400Hz )
     * 1 LSb = (8 * N[LSb]   + 1) / ODR       ( @ ODR=400Hz )
     * 
     * So, 5000 ms => (8 * N[2.5] + 1) / ODR    ; solve N=time(ms)/2.5
     *             => (8 * 2000[LSb] + 1) / ODR  
     *             => (8 * 2000 + 1) / ODR
     *             => 16001 / 400
     *             => 40
     * 
     * After `act_ths' worth of acceleration has been felt;
     * Stay in Wake mode for 5000 ms
     */

    /* This controls return-to-sleep */
    lis.cfg.act_dur = 40; /* 8-bit ; range 0-255  */

    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    for ( ;; ) {

        /* wait until INT2 goes active (Wake mode) */
        if (int_poll(GPIO_INTERRUPT_PIN_INT2)) {
            /* error handling */
        }

        /* Do "work" here at high ODR, HR mode, etc. */
        puts("I am awake!");
    
        /* wait until INT2 goes inactive (Sleep mode) */
        if (int_poll(GPIO_INTERRUPT_PIN_INT2)) {
            /* error handling */
        }

        puts("Sleeping...");
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}