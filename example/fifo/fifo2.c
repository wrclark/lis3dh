#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data data;
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_NORMAL;
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_AUTORESET;
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;

    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    if (lis3dh_poll_fifo(&lis)) {
        /* error handling */
    }

    if (lis3dh_read_fifo(&lis, &data)) {
        /* error handling */
    }

    /* read out fifo buffer data */
    for(i=0; i<data.size; i++) {
        printf("x: %f, y: %f, z: %f\n", data.x[i], data.y[i], data.z[i]);
    }

    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}