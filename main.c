#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "spi.h"

int main() {

    lis3dh_t lis;

    lis.dev.init = spi_init;
    lis.dev.read = spi_read;
    lis.dev.write = spi_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = spi_deinit;

    lis3dh_init(&lis);
    lis3dh_reset(&lis);

    lis.cfg.mode = LIS3DH_MODE_LP;
    lis.cfg.range = LIS3DH_FS_16G;
    lis.cfg.rate = LIS3DH_ODR_LP_5376_HZ;

    lis3dh_configure(&lis);

    for ( ;; ) {

        lis3dh_read(&lis);
        printf("x: %4.4d mg, y: %4.4d mg, z: %4.4d mg\n", lis.acc.x, lis.acc.y, lis.acc.z);
    }

    lis3dh_deinit(&lis);

    return 0;
}