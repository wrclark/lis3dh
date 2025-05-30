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
#include "interrupt.h"
#include "i2c.h"

#define GPIO_INTERRUPT_PIN_INT1 12

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

    /* register interrupt */
    if (int_register(GPIO_INTERRUPT_PIN_INT1)) {
        /* error handling */
    }

    /* set up config */
    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ; /* minimum recommended ODR */
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_NORMAL;
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;
    lis.cfg.filter.click = 1; /* enable filtering for CLICK function */

    lis.cfg.click.xs = 1; /* enable X axis single click */
    lis.cfg.click.ys = 1; /* enable Y axis single click */
    lis.cfg.click.zs = 1; /* enable Z axis single click */

    lis.cfg.pin1.click = 1; /* enable CLICK INT through pin1 */

    /* 1 LSb = 16 mg @ FS_2G 
     * so a 0.3g 'shock' is 300/16 = 18.75
     * However, the device can have up to +- 40mg read error, add 40 mg
     * 0.34g => 340/16 ~= 21
     * (Note: 0.34g and not 1.34g because of HP filter)
     */
    lis.cfg.click_ths = 21;

    /*
     * Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   400    2.5
     * 
     * At 400 ODR,
     * 20 ms = 20/2.5 = 8
     */
    lis.cfg.time_limit = 8;
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    /* read REFERENCE to set filter to current accel field */
    if (lis3dh_reference(&lis)) {
        /* error handling */
    }

    /* read CLICK_SRC to clear previous interrupts, if any */
    if (lis3dh_read_click(&lis)) {
        /* error handling */
    }

    for(;;) {

        /* poll interrupt on INT1 pin */
        if (int_poll(GPIO_INTERRUPT_PIN_INT1)) {
            /* error handling */
        }

        /* read CLICK_SRC when interrupt has fired */
        if (lis3dh_read_click(&lis)) {
            /* error handling */
        }

         /* only print if SCLICK=1 */
        if (LIS3DH_CLICK_SCLICK(lis.src.click)) {
            /* print data gathered from CLICK_SRC */
            printf("Click: X=%d, Y=%d, Z=%d, Sign=%d, S_CLICK=%d, D_CLICK=%d\n",
                LIS3DH_CLICK_SRC_X(lis.src.click),
                LIS3DH_CLICK_SRC_Y(lis.src.click),
                LIS3DH_CLICK_SRC_Z(lis.src.click),
                LIS3DH_CLICK_SIGN(lis.src.click),
                LIS3DH_CLICK_SCLICK(lis.src.click),
                LIS3DH_CLICK_DCLICK(lis.src.click));
        }
    }
    
    /* unregister interrupt */
    if (int_unregister(GPIO_INTERRUPT_PIN_INT1)) {
        /* error handling */
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}