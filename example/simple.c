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

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* device sometimes corrupts itself, so reset .. */
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    if (lis3dh_poll(&lis)) {
        /* error handling */
    }

    if (lis3dh_read(&lis)) {
        /* error handling */
    }

    printf("x: %f, y: %f, z: %f\n", lis.acc.x, lis.acc.y, lis.acc.z);

    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}