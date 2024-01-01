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

    /* reset device because it sometimes corrupts itself */
    if (lis3dh_reset(&lis)) {
        /* error handling */
    }

    /* set up config */
    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_400_HZ;
    lis.cfg.en_adc = 1; /* enable ADC */
    lis.cfg.en_temp = 1; /* enable temp sensing */

    
    /* write device config */
    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    if (lis3dh_read_adc(&lis)) {
        /* error handling */
    }

    /* ADC3 now reports the temperature instead of reading from its input */
    /* the temperature data is overwritten to lis.adc.adc3 */
    if (lis3dh_read_temp(&lis)) {
        /* error handling */
    }

    printf("ADC1: %04.04f mV\n", lis.adc.adc1);
    printf("ADC2: %04.04f mV\n", lis.adc.adc2);
    
    /* no decimals, step size is 1 celsius */
    printf("ADC3: %0.f oC\n", lis.adc.adc3);

    /* deinitalise struct */
    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}