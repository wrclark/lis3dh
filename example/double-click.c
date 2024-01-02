/*
 *       SCLICK                     SCLICK
 *     _________                 __________
 *     |        |                |       | |
 *     |        |                |       | |
 * -----        ------------------       | ------
 *                                       |
 *  TIME_LIMIT                 TIME_LIMIT|
 * >---------<               >----------<|
 *      LATENCY       WINDOW             | 
 *    >----------<>----------<           | => DCLICK INT
 * */
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

    /* reset device because it sometimes corrupts itself */
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
    lis.cfg.click.xd = 1; /* enable X axis double click */
    lis.cfg.click.yd = 1; /* enable Y axis double click */
    lis.cfg.click.zd = 1; /* enable Z axis double click */
    lis.cfg.pin1.click = 1; /* enable click int src through pin1 */
    lis.cfg.int1.latch = 1; /* latch interrupt until INT1_SRC is read */

    /* 1 LSb = 16 mg @ FS_2G 
     * so a 0.072g 'shock' is 72/16 = 4.5
     * However, the device can have up to +- 40mg read error
     * 0.112g => 112/16 = 15
     */
    lis.cfg.click_ths = 15; /* pretty sensitive */

    /* Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   400    2.5
     * 
     *  For ODR=400:
     *  time_limit of 75 ms = 75/2.5 = 30
     *  time_latency of 40 ms = 40/2.5 = 16
     *  time_window of 500 ms = 500/2.5 = 200 
     * 
     */
    lis.cfg.time_limit = 30; /* range: 0-127 */
    lis.cfg.time_latency = 16; /* range: 0-255 */
    lis.cfg.time_window = 200; /* range: 0-255 */
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
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

        /* only print data if DCLICK=1 in CLICK_SRC */
        if (LIS3DH_CLICK_DCLICK(lis.src.click)) {
            /* print data gathered from CLICK_SRC */
            printf("Click: X=%d, Y=%d, Z=%d, Sign=%d, S_en=%d, D_en=%d\n",
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