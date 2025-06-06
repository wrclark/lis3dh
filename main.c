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


#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lis3dh.h"
#include "i2c.h"
/* #include "spi.h" */

int main(void) {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    /*
    lis.dev.init = spi_init;
    lis.dev.read = spi_read;
    lis.dev.write = spi_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = spi_deinit;
    */

    lis3dh_init(&lis);
    lis3dh_reset(&lis);

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_FIFO;

    lis3dh_configure(&lis);

    for ( ;; ) {

        usleep(100000); /* 100 ms */
        lis3dh_read_fifo(&lis, &fifo);
        for(i=0; i<fifo.size; i++)
            printf("x: %4.4d mg, y: %4.4d mg, z: %4.4d mg\n", fifo.x[i], fifo.y[i], fifo.z[i]);

        printf("fifo.size=%d\n", fifo.size);
        lis3dh_fifo_reset(&lis);
    }

    lis3dh_deinit(&lis);

    return 0;
}
