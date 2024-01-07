#define _GNU_SOURCE /* usleep() */
#include <unistd.h> /* usleep() */
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    /* initialise struct and check device id */
    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* reset just in case*/
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM;

    /* modifying the size of the FIFO buffer is not useful when just polling without interrupts */

    /* write cfg to device */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    for ( ;; ) {

        /* read FIFO into `fifo_data' struct `fifo' */
        if (lis3dh_read_fifo(&lis, &fifo)) {
            /* error handling */
        }

        /* print data .. */
        for(i=0; i<fifo.size; i++) {
            printf("x: %4.4d mg, y: %4.4d mg, z: %4.4d mg\n", fifo.x[i], fifo.y[i], fifo.z[i]);
        }

        /* FIFO size will rarely go above 2 in this example, because lis3dh_read_fifo() */
        /* does not wait for the FIFO buffer to be full, just non-empty. */
        printf("fifo.size=%d\n", fifo.size);

        /* No need to reset the FIFO engine in stream mode */
    }

    /* deinitalise device */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}