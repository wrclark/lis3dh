#define _GNU_SOURCE
#include <unistd.h> /* usleep() */
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

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

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    /* normal mode */
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_NORMAL;
    /* 3dB cutoff freq (~ ODR) */
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;

    /* write config to device */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* read REFERENCE to set filter to current accel field */
    if (lis3dh_reference(&lis)) {
        /* error handling */
    }

    for ( ;; ) {

        /* read an [x y z] accel set. Filter is applied */
        if (lis3dh_read(&lis)) {
            /* error handling */
        }

        printf("x: %4.d, y: %4.d, z: %4.d\n", lis.acc.x, lis.acc.y, lis.acc.z);
        usleep(5000); /* 5 ms */
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}