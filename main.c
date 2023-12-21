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


    if (!lis3dh_init(&lis)) {
        puts("init OK");
    }

    lis.cfg.mode = LIS3DH_MODE_NORMAL;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    if (!lis3dh_configure(&lis)) {
        puts("configure OK");
    }

    if (!lis3dh_poll(&lis)) {
        puts("poll OK");
    }

    if (!lis3dh_read(&lis)) {
        puts("read OK");
    }

    printf("x: %.3g, y: %.3g, z: %.3g\n", lis.acc.x, lis.acc.y, lis.acc.z);
    printf("mag: %.3g\n", accel_mag(&lis));

    if (!lis3dh_deinit(&lis)) {
        puts("deinit OK");
    }

    return 0;
}