#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "interrupt.h"
#include "i2c.h"

/* GPIO 12 or Pin 32 */
#define GPIO_INTERRUPT_PIN 12

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int k;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    /* initalise LIS3DH struct */
    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* reset device just in case */
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    /* register interrupt */
    if (int_register(GPIO_INTERRUPT_PIN)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_NORMAL;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM;
    lis.cfg.fifo.trig = LIS3DH_FIFO_TRIG_INT1; /* trigger interrupt into int pin1 */
    lis.cfg.pin1.wtm = 1; /* trigger upon FIFO watermark level reached */

    /* set up HP filter to remove DC component */
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_NORMAL_REF;
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_4;
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
       /* error handling */
    }

    /* read REFERENCE to set filter to current accel field */
    if (lis3dh_reference(&lis)) {
        /* error handling */
    }
    
    /* wait for interrupt from LIS3DH */
    if (int_poll(GPIO_INTERRUPT_PIN)) {
        /* error handling */
    }

    /* read as many [x y z] sets as specified by watermark level (fth) */
    /* copy them to the fifo data struct given below as `fifo' */
    if (lis3dh_read_fifo(&lis, &fifo)) {
       /* error handling */
    }

    /* above function also writes out the qty of [x y z] sets stored in `fifo' */
    for(k=0; k<fifo.size; k++) {
        printf("x: %04.04f, y: %04.04f z: %04.04f\n", fifo.x[k], fifo.y[k], fifo.z[k]);
    }
    
    /* unregister interrupt */
    if (int_unregister(GPIO_INTERRUPT_PIN)) {
        /* error handling */
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}