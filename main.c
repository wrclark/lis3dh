#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "interrupt.h"
#include "i2c.h"

#define GPIO_INTERRUPT_PIN_INT1 12
#define GPIO_INTERRUPT_PIN_INT2 16

/* print message then exit */
static void quit(const char *msg, lis3dh_t *lis) {
    lis->dev.deinit();
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

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
        quit("init()", &lis);
    }

    /* reset device because it sometimes corrupts itself */
    if (lis3dh_reset(&lis)) {
        quit("reset()", &lis);
    }

    /* register interrupt */
    if (int_register(GPIO_INTERRUPT_PIN_INT1)) {
        quit("int_register()", &lis);
    }

    /* set up config */
    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_NORMAL;
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;
    lis.cfg.click.latch = 1;
    lis.cfg.click.xs = 1;
    lis.cfg.click.ys = 1;
    lis.cfg.click.zs = 1;
    lis.cfg.pin1.click = 1;

    /* 1 LSb = 16 mg @ FS_2G 
     * so a 0.8g 'shock' is 800/16 = 50
     */
    lis.cfg.click_ths = 50;

    /* the 800 mg shock must be gone after 30 ms let's say .. */
        /* Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   400    2.5
     * 
     * At 400 ODR, 30ms/2.5ms = 16
     */
    lis.cfg.time_limit = 16;
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
        quit("configure()", &lis);
    }
    
    if (int_poll(GPIO_INTERRUPT_PIN_INT1)) {
        quit("int_poll()", &lis);
    }

    if (lis3dh_read_click(&lis)) {
        quit("read_click()", &lis);
    }

    /* print data gathered from CLICK_SRC */
    printf("Click: X=%d, Y=%d, Z=%d, Sign=%d, S_en=%d, D_en=%d\n",
        LIS3DH_CLICK_SRC_X(lis.src.click),
        LIS3DH_CLICK_SRC_Y(lis.src.click),
        LIS3DH_CLICK_SRC_Z(lis.src.click),
        LIS3DH_CLICK_SIGN(lis.src.click),
        LIS3DH_CLICK_SCLICK(lis.src.click),
        LIS3DH_CLICK_DCLICK(lis.src.click));
    
    /* unregister interrupt */
    if (int_unregister(GPIO_INTERRUPT_PIN_INT1)) {
        quit("int_unregister()", &lis);
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        quit("deinit()", &lis);
    }

    return 0;
}