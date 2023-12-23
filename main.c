#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "i2c.h"

float mag(float x, float y, float z) {
    return sqrt( powf(x, 2) + powf(y, 2) + powf(z, 2) );
}

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;


    /* write config after init() */
    if (lis3dh_init(&lis)) {
        puts("init ERR");
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM;

    if (lis3dh_configure(&lis)) {
        puts("configure ERR");
    }

    for(int i=0; i<100; i++) {
    
        if (lis3dh_poll_fifo(&lis)) {
            puts("poll_fifo ERR");
        }

        if (lis3dh_read_fifo(&lis, &fifo)) {
            puts("read_fifo ERR");
        }

        for(int k=0; k<fifo.size; k++) {
            printf("x: %04.04f, y: %04.04f, z: %04.04f mag: %04.04f\n",
                fifo.x[k], fifo.y[k], fifo.z[k],
                mag(fifo.x[k], fifo.y[k], fifo.z[k]));
        }
   }
    
    if (lis3dh_deinit(&lis)) {
        puts("deinit ERR");
    }

    return 0;
}