/*
 * This file is part of lis3dh.
 *
 * Copyright (C) 2025 William Clark
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#define _GNU_SOURCE /* usleep() */
#include <unistd.h> /* usleep() */
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int i;

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

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_FIFO;

    /* modifying the size of the FIFO buffer is not useful when just polling without interrupts */

    /* write cfg to device */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    for ( ;; ) {

        /* read FIFO into `fifo_data' struct `fifo' */
        if (lis3dh_read_fifo(&lis, &fifo)) {
            /* error handling */
        }

        /* print data .. */
        for(i=0; i<fifo.size; i++) {
            printf("x: %4.4d mg, y: %4.4d mg, z: %4.4d mg\n", fifo.x[i], fifo.y[i], fifo.z[i]);
        }

        /* FIFO size will rarely go above 2 in this example, because lis3dh_read_fifo() */
        /* does not wait for the FIFO buffer to be full, just non-empty. */
        printf("fifo.size=%d\n", fifo.size);

        /* Since the WTM is less than the full size, there is no need to reset the device after reading */
        /* However, if the reading of the device is somehow delayed and the FIFO fills up between */
        /* calls to lis3dh_read_fifo(), then the device is overrun, and must be reset. */
        /* this can easily be simulated by calling a sleep function. */
        /* Therefore, it is a good idea to reset the FIFO anyway. */
        lis3dh_fifo_reset(&lis);
    }

    /* deinitalise device */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}