#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "i2c.h"

/* calc magnitude of accel [x y z] vector */
float mag(float x, float y, float z) {
    return sqrt( powf(x, 2) + powf(y, 2) + powf(z, 2) );
}

/* print message then exit */
void quit(const char *msg, lis3dh_t *lis) {
    lis->dev.deinit();
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int i, k;

    /* set fn ptrs to rw on bus (i2c or SPI) */
    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;


    /* initialise struct */
    if (lis3dh_init(&lis)) {
        quit("init()", &lis);
    }

    /* set up config */
    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM;

    /* write device config */
    if (lis3dh_configure(&lis)) {
        quit("configure()", &lis);
    }

    for (i=0; i<50; i++) {
    
        /* poll fifo reg */
        if (lis3dh_poll_fifo(&lis)) {
            quit("poll_fifo()", &lis);
        }

        /* read stored fifo data into `fifo' struct */
        if (lis3dh_read_fifo(&lis, &fifo)) {
            quit("read_fifo()", &lis);
        }

        for(k=0; k<fifo.size; k++) {
            printf("x: %04.04f, y: %04.04f, z: %04.04f mag: %04.04f\n",
                fifo.x[k], fifo.y[k], fifo.z[k],
                mag(fifo.x[k], fifo.y[k], fifo.z[k]));
        }
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        quit("deinit()", &lis);
    }

    return 0;
}