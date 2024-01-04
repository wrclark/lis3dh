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
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* poll STATUS_REG for new [x y z] data ready */
    if (lis3dh_poll(&lis)) {
        /* error handling */
    }

    /* read latest [x y z] data, store in the `lis' struct's `acc' field */
    if (lis3dh_read(&lis)) {
        /* error handling */
    }

    printf("x: %d mg, y: %d mg, z: %d mg\n", lis.acc.x, lis.acc.y, lis.acc.z);

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}