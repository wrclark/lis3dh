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
    lis.cfg.click.xs = 1; /* enable X axis single click */
    lis.cfg.click.ys = 1; /* enable Y axis single click */
    lis.cfg.click.zs = 1; /* enable Z axis single click */
    lis.cfg.pin1.click = 1; /* enable click int src through pin1 */
    lis.cfg.int1.latch = 1; /* latch interrupt until INT1_SRC is read */

    /* 1 LSb = 16 mg @ FS_2G 
     * so a 0.072g 'shock' is 72/16 = 4.5
     * However, the device can have up to +- 40mg read error
     * 0.112g => 112/16 = 7
     */
    lis.cfg.click_ths = 7; /* pretty sensitive */

    /* the 'shock' must be gone after 10 ms let's say ..*/
    /*
     * Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   400    2.5
     * 
     * At 400 ODR, 10ms/2.5ms = 4
     */
    lis.cfg.time_limit = 4;
    
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

        /* print data gathered from CLICK_SRC */
        printf("Click: X=%d, Y=%d, Z=%d, Sign=%d, S_en=%d, D_en=%d\n",
            LIS3DH_CLICK_SRC_X(lis.src.click),
            LIS3DH_CLICK_SRC_Y(lis.src.click),
            LIS3DH_CLICK_SRC_Z(lis.src.click),
            LIS3DH_CLICK_SIGN(lis.src.click),
            LIS3DH_CLICK_SCLICK(lis.src.click),
            LIS3DH_CLICK_DCLICK(lis.src.click));
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