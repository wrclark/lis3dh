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

    /* no filter because we want the 'absolute' experienced acceleration */

    lis.cfg.pin1.ia1 = 1; /* enable INT1 through pin1 */
    lis.cfg.int1.latch = 1; /* latch interrupt until INT1_SRC is read */

    /* enable all the low directions */
    /* this works because the low directions are compared to -threshold, */
    /* and are below it at rest, and greater than it at free-fall, */
    /* while the opposite is true for the high directions. */
    lis.cfg.int1.xl = 1;
    lis.cfg.int1.yl = 1;
    lis.cfg.int1.zl = 1;

    /* AND interrupt mode */
    lis.cfg.int1.aoi = 1;
    lis.cfg.int1.en_6d = 0;

    /* 1 LSb = 16 mg @ FS_2G 
     * Due to inherent problems with accuracy and timing, you should use a "free-fall window",
     * rather than setting the threshold to 0. AN3308 pg 27 recommends at or below 350mg for all axes,
     * to be considered as being in a state of free-fall, but the gradual return to experiencing
     * 1g in free-fall is not discussed.
     * 
     * 350/16 ~= 20
     */
    lis.cfg.int1_ths = 20;

    /*
     * Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   400    2.5
     * 
     * At 400 ODR,
     * 30 ms = 30/2.5 = 12
     */
    lis.cfg.int1_dur = 12;
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* read INT1_SRC to clear previous interrupts, if any */
    if (lis3dh_read_int1(&lis)) {
        /* error handling */
    }

    for ( ;; ) {

         /* poll interrupt on INT1 pin */
        if (int_poll(GPIO_INTERRUPT_PIN_INT1)) {
            /* error handling */
        }

        /* read INT1_SRC when interrupt has fired */
        if (lis3dh_read_int1(&lis)) {
            /* error handling */
        }

        /* only print if IA=1 */
        if (LIS3DH_INT_SRC_IA(lis.src.int1)) {
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