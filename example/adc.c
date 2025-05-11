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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;

    /* set fn ptrs to rw on bus (i2c or SPI) */
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

    /* set up config */
    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;
    lis.cfg.en_adc = 1; /* enable ADC */

    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* Read all 3 ADCs */
    if (lis3dh_read_adc(&lis)) {
        /* error handling */
    }

    /* print measured mV */
    printf("ADC1: %d mV\n", lis.adc.adc1);
    printf("ADC2: %d mV\n", lis.adc.adc2);
    printf("ADC3: %d mV\n", lis.adc.adc3);

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}