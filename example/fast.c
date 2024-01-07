#define _GNU_SOURCE /* usleep() */
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

    /* initialise struct and check device id */
    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* reset just in case*/
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_LP; /* Low-power mode */
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_LP_5376_HZ; /* 5 KHz ODR, only available in low-power mode */

    /* write cfg to device */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    for ( ;; ) {

        /* read accelerometer and store results at struct lis.acc */
        if (lis3dh_read(&lis)) {
            /* error handling */
        }

        /* print data .. */
        printf("x: %4.4d mg, y: %4.4d mg, z: %4.4d mg\n", lis.acc.x, lis.acc.y, lis.acc.z);
    }

    /* deinitalise device */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}