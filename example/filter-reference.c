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
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    /* normal mode */
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_REFERENCE;
    /* 3dB cutoff freq (~ ODR) */
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;

    /* Output [x y z] data is calculated as the difference between the 
    * measured acceleration and the value stored in REFERENCE.
    * signed 7-bit int, 1 LSb value depends on FS.
    * FS_2G: ~ 16mg per 1 LSb 
    * FS_4G: ~ 31mg per 1 LSb 
    * FS_8G: ~ 63mg per 1 LSb
    * FS_16G: ~127mg per 1 LSb */
    /* @ 2G,
     * To have the output data be referenced to 0.8g:
     * => 800 mg / 16 mg = 50 */
    /* to reference -0.8g simply put -50 */
    lis.cfg.reference = 50;

    /* write config to device */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* Do not read REFERENCE */

    for ( ;; ) {

        /* read an [x y z] accel set. Filter is applied */
        if (lis3dh_read(&lis)) {
            /* error handling */
        }

        printf("x: %4.d, y: %4.d, z: %4.d\n", lis.acc.x, lis.acc.y, lis.acc.z);
        usleep(5000); /* 5 ms */
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}