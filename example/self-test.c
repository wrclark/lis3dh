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
#include <unistd.h> /* usleep() */
#include <stdio.h>
#include <stdlib.h>
#include "lis3dh.h"
#include "i2c.h"

/* table 4 of datasheet */
#define ST_MIN 17
#define ST_MAX 360

/* AN3308 suggests 5 */
#define ITERATIONS 5

int main() {
    
    lis3dh_t lis;
    int32_t x_nost, y_nost, z_nost; /* store avg of measurements before self-test */
    int32_t x_st, y_st, z_st; /* store avg of self-test measurements */
    int32_t xd, yd, zd; /* differences */
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    x_nost = y_nost = z_nost = 0;
    x_st = y_st = z_st = 0;
    xd = yd = zd = 0;

    /* initialise LIS3DH struct */
    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* reset device just in case */
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_NORMAL;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_50_HZ;

    lis3dh_configure(&lis);

    /* discard first couple of samples as datasheet suggests they are wrong/noisy */
    for(i=0; i<15; i++) lis3dh_read(&lis);

    for(i=0; i<ITERATIONS; i++) {
        lis3dh_read(&lis);
        x_nost += lis.acc.x;
        y_nost += lis.acc.y;
        z_nost += lis.acc.z;
    }

    x_nost /= ITERATIONS;
    y_nost /= ITERATIONS;
    z_nost /= ITERATIONS;

    lis.cfg.self_test = LIS3DH_SELF_TEST_0;

    lis3dh_configure(&lis);

    /* discard first couple of samples as datasheet suggests they are wrong/noisy */
    for(i=0; i<15; i++) lis3dh_read(&lis);


    for(i=0; i<ITERATIONS; i++) {
        lis3dh_read(&lis);
        x_st += lis.acc.x;
        y_st += lis.acc.y;
        z_st += lis.acc.z;
    }

    x_st /= ITERATIONS;
    y_st /= ITERATIONS;
    z_st /= ITERATIONS;

    xd = abs(x_st - x_nost);
    yd = abs(y_st - y_nost);
    zd = abs(z_st - z_nost);

    printf("variance min=%d, max=%d\n", ST_MIN, ST_MAX);
    printf("xd=%d yd=%d zd=%d\n", xd, yd, zd);

    if ((ST_MIN <= xd && xd <= ST_MAX) && (ST_MIN <= yd && yd <= ST_MAX) && (ST_MIN <= zd && zd <= ST_MAX)) {
        puts("Pass");
    } else {
        puts("Fail");
    }

    lis3dh_reset(&lis);

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}