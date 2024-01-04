#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "interrupt.h"
#include "i2c.h"

/* GPIO 12 or Pin 32 */
#define GPIO_INTERRUPT_PIN 12

static void quit(const char *msg, lis3dh_t *lis) {
    lis->dev.deinit();
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static float mag(float x, float y, float z) {
    return (float) sqrt(x*x + y*y + z*z);
}

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int k;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    /* initalise LIS3DH struct */
    if (lis3dh_init(&lis)) {
        quit("init()", &lis);
    }

    /* reset device just in case */
    if (lis3dh_reset(&lis)) {
        quit("reset()", &lis);
    }

    /* register interrupt */
    if (int_register(GPIO_INTERRUPT_PIN)) {
        quit("int_register()", &lis);
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;

    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM_TO_FIFO;
    lis.cfg.fifo.trig = LIS3DH_FIFO_TRIG_INT1; /* trigger interrupt into int pin1 */
    lis.cfg.pin1.wtm = 1; /* trigger upon FIFO watermark level reached */

    lis.cfg.en_adc =1;
    lis.cfg.en_temp = 1;
    
    /* write device config */
    if (lis3dh_configure(&lis)) {
       quit("configure()", &lis);
    }
    
    /* wait for interrupt from LIS3DH */
    if (int_poll(GPIO_INTERRUPT_PIN)) {
     
        quit("int_poll()", &lis);
    }

    /* read as many [x y z] sets as specified by watermark level (fth) */
    /* copy them to the fifo data struct given below as `fifo' */
    if (lis3dh_read_fifo(&lis, &fifo)) {
       quit("read_fifo()", &lis);
    }

    /* above function also writes out the qty of [x y z] sets stored in `fifo' */
    for(k=0; k<fifo.size; k++) {
        printf("x: %d, y: %d z: %d\n",
            fifo.x[k], fifo.y[k], fifo.z[k]);
    }

    printf("mag: %04.04f\n", mag(100.0, 45.0, 120.0));


    if (lis3dh_read_adc(&lis)) {
        quit("adc()", &lis);
    }

    if (lis3dh_read_temp(&lis)) {
        quit("temp()", &lis);
    }

    printf("ADC1: %d mV, temp: %d\n", lis.adc.adc1, lis.adc.adc3);
    
    /* unregister interrupt */
    if (int_unregister(GPIO_INTERRUPT_PIN)) {
       quit("int_unregister()", &lis);
    }

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        quit("deinit()", &lis);
    }

    return 0;
}