#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "i2c.h"

double accel_mag(lis3dh_t *lis) {
    double d = 0.0;

    d = sqrt( powf(lis->acc.x, 2) + powf(lis->acc.y, 2) + powf(lis->acc.z, 2));
    return d;
}

int main() {

    lis3dh_t lis;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;


    /* write config after init() */
    if (lis3dh_init(&lis)) {
        puts("init ERR");
    }

    lis.cfg.mode = LIS3DH_MODE_NORMAL;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_NORMAL;

    if (lis3dh_configure(&lis)) {
        puts("configure ERR");
    }

/*
    for(int i=0; i<100; i++) {
        if (lis3dh_poll(&lis)) {
            puts("poll ERR");
        }

        if (lis3dh_read(&lis)) {
            puts("read ERR");
        }

        printf("x: % 04.04f g, y: % 04.04f g, z: % 04.04f g, mag:% 04.04f\n",
            lis.acc.x, lis.acc.y, lis.acc.z, accel_mag(&lis));
    }
    */

    /* FIFO test */
    if (lis3dh_poll_fifo(&lis)) {
        puts("poll_fifo ERR");
    }

    if (lis3dh_read_fifo(&lis)) {
        puts("read_fifo ERR");
    }
    
    if (lis3dh_deinit(&lis)) {
        puts("deinit ERR");
    }

    return 0;
}