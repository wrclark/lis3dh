#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "interrupt.h"
#include "i2c.h"

#define GPIO_INTERRUPT_PIN_INT1 12

int main() {

    lis3dh_t lis;

    /* set fn ptrs to rw on bus (i2c or SPI) */
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

    /* register interrupt */
    if (int_register(GPIO_INTERRUPT_PIN_INT1)) {
        /* error handling */
    }

    /* set up config */
    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;

    lis.cfg.pin1.ia1 = 1; /* allow INT1 through INT_PIN1 */
    

    /* 1 LSb = 16 mg @ FS_2G 
     * 0.3g threshold = 300/16 = 18.75
     * add read error, +40mg => 340/16 = 21.25 ~= 21
     * if you for some reason don't want to use the HP filter,
     * just add 1g to the threshold calculation.
     */
    lis.cfg.int1_ths = 21;

    /* Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   400    2.5
     * 
     *  For ODR=400:
     *     10 ms => 10/2.5 = 5
     * lis.cfg.int1_dur = 5;  <== 10 ms minimum duration to wake up 
     */
    lis.cfg.int1_dur = 0; /* instantaneous */

    /* enable X_high, Y_high and Z_high */
    lis.cfg.int1.yh = 1;
    lis.cfg.int1.zh = 1;
    lis.cfg.int1.xh = 1;

    /* OR mode. Think about the axis combinations for AND mode */
    lis.cfg.int1.aoi = 0; /* set to 1 for AND mode */
    lis.cfg.int1.en_6d = 0;
    
    /* latch interrupt */
    lis.cfg.int1.latch = 1;

    /* set up a HP filter to ignore constant earth acceleration */
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_NORMAL;
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;
    lis.cfg.filter.ia1 = 1; /* enable filter for INT1 generator */
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* read REFERENCE to set filter to current accel field */
    if (lis3dh_reference(&lis)) {
        /* error handling */
    }

    /* read INT1_SRC to clear old interrupt if any */
    if (lis3dh_read_int1(&lis)) {
        /* error handling */
    }

    for( ;; ) {

        /* wait for INT1 to go active */
        if (int_poll(GPIO_INTERRUPT_PIN_INT1)) {
            /* error handling */
        }

        /* read INT1_SRC */
        if (lis3dh_read_int1(&lis)) {
            /* error handling */
        }

        /* print received interrupt .. */
        printf("IA=%d ZH=%d ZL=%d YH=%d YL=%d XH=%d XL=%d\n",
            LIS3DH_INT_SRC_IA(lis.src.int1),
            LIS3DH_INT_SRC_Z_HIGH(lis.src.int1),
            LIS3DH_INT_SRC_Z_LOW(lis.src.int1),
            LIS3DH_INT_SRC_Y_HIGH(lis.src.int1),
            LIS3DH_INT_SRC_Y_LOW(lis.src.int1),
            LIS3DH_INT_SRC_X_HIGH(lis.src.int1),
            LIS3DH_INT_SRC_X_LOW(lis.src.int1));
    }
    
    /* unregister interrupt */
    if (int_unregister(GPIO_INTERRUPT_PIN_INT1)) {
       /* error handling */
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}