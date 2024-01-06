#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"
#include "spi.h"

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data data;
    int i;

    lis.dev.init = spi_init;
    lis.dev.read = spi_read;
    lis.dev.write = spi_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = spi_deinit;

    /* initialise LIS3DH struct */
    if (lis3dh_init(&lis)) {
        puts("init()");
    }

    /* reset device just in case */
    if (lis3dh_reset(&lis)) {
        puts("reset()");
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM_TO_FIFO;
    lis.cfg.fifo.size = 20;

    /* write device config */
    if (lis3dh_configure(&lis)) {
        puts("configure()");
    }

A:
    /* poll fifo register until it reports that the watermark level has 
       been reached, or that it has overwritten old data, whichever 
       happens first. */
    if (lis3dh_poll_fifo(&lis)) {
        puts("poll_fifo()");
    }

    /* read as many [x y z] sets as specified by watermark level (fth) */
    /* copy them to the fifo data struct given below as `fifo' */
    if (lis3dh_read_fifo(&lis, &data)) {
        puts("read_fifo()");
    }

    /* read out fifo buffer data */
    for(i=0; i<data.size; i++) {
        printf("x: %d mg, y: %d mg, z: %d mg\n", data.x[i], data.y[i], data.z[i]);
    }
goto A;
    /* deinitialise struct */
    if (lis3dh_deinit(&lis)) {
       puts("deinit()");
    }

    return 0;
}