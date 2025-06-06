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


#ifndef LIS3DH_H
#define LIS3DH_H

#include <stdint.h>

/* register map*/
#define REG_STATUS_REG_AUX 0x07 /* ro */
#define REG_OUT_ADC1_L     0x08 /* ro */
#define REG_OUT_ADC1_H     0x09 /* ro */
#define REG_OUT_ADC2_L     0x0A /* ro */
#define REG_OUT_ADC2_H     0x0B /* ro */
#define REG_OUT_ADC3_L     0x0C /* ro */
#define REG_OUT_ADC3_H     0x0D /* ro */
#define REG_WHO_AM_I       0x0F /* ro; default=0x33 */
#define REG_CTRL_REG0      0x1E /* rw; default=0x10 */
#define REG_TEMP_CFG_REG   0x1F /* rw; default=0x00 */
#define REG_CTRL_REG1      0x20 /* rw; default=0x07 */
#define REG_CTRL_REG2      0x21 /* rw; default=0x00 */
#define REG_CTRL_REG3      0x22 /* rw; default=0x00 */
#define REG_CTRL_REG4      0x23 /* rw; default=0x00 */
#define REG_CTRL_REG5      0x24 /* rw; default=0x00 */
#define REG_CTRL_REG6      0x25 /* rw; default=0x00 */
#define REG_REFERENCE      0x26 /* rw; default=0x00 */
#define REG_STATUS_REG     0x27 /* ro */
#define REG_OUT_X_L        0x28 /* ro */
#define REG_OUT_X_H        0x29 /* ro */
#define REG_OUT_Y_L        0x2A /* ro */
#define REG_OUT_Y_H        0x2B /* ro */
#define REG_OUT_Z_L        0x2C /* ro */
#define REG_OUT_Z_H        0x2D /* ro */
#define REG_FIFO_CTRL_REG  0x2E /* rw; default=0x00 */
#define REG_FIFO_SRC_REG   0x2F /* ro */
#define REG_INT1_CFG       0x30 /* rw; default=0x00 */
#define REG_INT1_SRC       0x31 /* ro */
#define REG_INT1_THS       0x32 /* rw; default=0x00 */
#define REG_INT1_DURATION  0x33 /* rw; default=0x00 */
#define REG_INT2_CFG       0x34 /* rw; default=0x00 */
#define REG_INT2_SRC       0x35 /* ro */
#define REG_INT2_THS       0x36 /* rw; default=0x00 */
#define REG_INT2_DURATION  0x37 /* rw; default=0x00 */
#define REG_CLICK_CFG      0x38 /* rw; default=0x00 */
#define REG_CLICK_SRC      0x39 /* ro */
#define REG_CLICK_THS      0x3A /* rw; default=0x00 */
#define REG_TIME_LIMIT     0x3B /* rw; default=0x00 */
#define REG_TIME_LATENCY   0x3C /* rw; default=0x00 */
#define REG_TIME_WINDOW    0x3D /* rw; default=0x00 */
#define REG_ACT_THS        0x3E /* rw; default=0x00 */
#define REG_ACT_DUR        0x3F /* rw; default=0x00 */

/* macros for checking INT_SRC registers upon interrupt */
#define LIS3DH_INT_SRC_X_LOW(c)   (!!(c & 0x01)) /* X axis low */
#define LIS3DH_INT_SRC_X_HIGH(c)  (!!(c & 0x02)) /* X axis high */
#define LIS3DH_INT_SRC_Y_LOW(c)   (!!(c & 0x04)) /* Y axis low */
#define LIS3DH_INT_SRC_Y_HIGH(c)  (!!(c & 0x08)) /* Y axis high */
#define LIS3DH_INT_SRC_Z_LOW(c)   (!!(c & 0x10)) /* Z axis low */
#define LIS3DH_INT_SRC_Z_HIGH(c)  (!!(c & 0x20)) /* Z axis high */
#define LIS3DH_INT_SRC_IA(c)      (!!(c & 0x40)) /* Interrupt active */

/* macros for checking CLICK_SRC register upon interrupt */
#define LIS3DH_CLICK_SRC_X(c)     (!!(c & 0x01)) /* X high event */
#define LIS3DH_CLICK_SRC_Y(c)     (!!(c & 0x02)) /* Y high event */
#define LIS3DH_CLICK_SRC_Z(c)     (!!(c & 0x04)) /* Z high event */
#define LIS3DH_CLICK_SIGN(c)      (!!(c & 0x08)) /* Click sign, 1 = positive */
#define LIS3DH_CLICK_SCLICK(c)    (!!(c & 0x10)) /* Single-click det. enabled */
#define LIS3DH_CLICK_DCLICK(c)    (!!(c & 0x20)) /* Double-click det. enabled */

/* macros for checking FIFO_SRC register */
#define LIS3DH_FIFO_SRC_UNREAD(c)    (c & 0x1F)  /* FSS / current unread samples in FIFO */
#define LIS3DH_FIFO_SRC_EMPTY(c)  (!!(c & 0x20)) /* FIFO is empty */
#define LIS3DH_FIFO_SRC_OVRN(c)   (!!(c & 0x40)) /* FIFO has overrun (full 32) */
#define LIS3DH_FIFO_SRC_WTM(c)    (!!(c & 0x80)) /* FIFO watermark hit (not full 32) */

/* macros for checking STATUS register */
#define LIS3DH_STATUS_XDA(c)      (!!(c & 0x01)) /* X-axis data available */
#define LIS3DH_STATUS_YDA(c)      (!!(c & 0x02)) /* Y-axis data available */
#define LIS3DH_STATUS_ZDA(c)      (!!(c & 0x04)) /* Z-axis data available */
#define LIS3DH_STATUS_ZYXDA(c)    (!!(c & 0x08)) /* {Z,Y,X}-axis data available */
#define LIS3DH_STATUS_XOR(c)      (!!(c & 0x10)) /* X-axis data has overrun (been overwritten) */
#define LIS3DH_STATUS_YOR(c)      (!!(c & 0x20)) /* Y-axis data has overrun (been overwritten) */
#define LIS3DH_STATUS_ZOR(c)      (!!(c & 0x40)) /* Z-axis data has overrun (been overwritten) */
#define LIS3DH_STATUS_ZYXOR(c)    (!!(c & 0x80)) /* {Z,Y,X}-axis data has overrun (been overwritten) */


/* rates */
/* all power modes */
#define LIS3DH_ODR_POWEROFF 0x00
#define LIS3DH_ODR_1_HZ  0x01
#define LIS3DH_ODR_10_HZ 0x02
#define LIS3DH_ODR_25_HZ 0x03
#define LIS3DH_ODR_50_HZ 0x04
#define LIS3DH_ODR_100_HZ 0x05
#define LIS3DH_ODR_200_HZ 0x06
#define LIS3DH_ODR_400_HZ 0x07
/* only normal mode */
#define LIS3DH_ODR_NORM_1344_HZ 0x09
/* only low-power mode */
#define LIS3DH_ODR_LP_1600_HZ 0x08
#define LIS3DH_ODR_LP_5376_HZ 0x09


/* range/sens */
#define LIS3DH_FS_2G  0x00 /* Full-scale sensing range: ± 2G */
#define LIS3DH_FS_4G  0x01 /* Full-scale sensing range: ± 4G */
#define LIS3DH_FS_8G  0x02 /* Full-scale sensing range: ± 8G */
#define LIS3DH_FS_16G 0x03 /* Full-scale sensing range: ± 16G */


/* operating modes */
#define LIS3DH_MODE_HR     0x00 /* High resolution: 12-bit */
#define LIS3DH_MODE_LP     0x01 /* Low-power: 8-bit */
#define LIS3DH_MODE_NORMAL 0x02 /* Normal: 10-bit */


/* FIFO modes */
/* FIFO is not operational, and the buffer is reset. Must be used for switching FIFO modes */
#define LIS3DH_FIFO_MODE_BYPASS         0x00
/* Once FIFO fills up completely, it must be reset to be used again */
#define LIS3DH_FIFO_MODE_FIFO           0x01 
/* Continously (over)writes buffer until it is read */
#define LIS3DH_FIFO_MODE_STREAM         0x02
/* Stream mode, but automatically switches to FIFO mode once a set interrupt has occurred */
#define LIS3DH_FIFO_MODE_STREAM_TO_FIFO 0x03

/* FIFO trigger pin selection */
#define LIS3DH_FIFO_TRIG_INT1 0x00
#define LIS3DH_FIFO_TRIG_INT2 0x01


/* Reference mode
 * Output [x y z] data is calculated as the difference between the 
 * measured acceleration and the value stored in REFERENCE.
 * signed 7-bit int, 1 LSb value depends on FS.
 * FS_2G: ~ 16mg per 1 LSb 
 * FS_4G: ~ 31mg per 1 LSb 
 * FS_8G: ~ 63mg per 1 LSb
 * FS_16G: ~127mg per 1 LSb */
#define LIS3DH_FILTER_MODE_REFERENCE  0x01

/* Normal mode
 * Probably the same as 0x00 */
#define LIS3DH_FILTER_MODE_NORMAL     0x02 

/* Autoreset mode
 * The filter is automatically reset upon configured interrupt event.
 * It can be reset at any time by reading REFERENCE. */
#define LIS3DH_FILTER_MODE_AUTORESET  0x03


/* filter cutoff */
/* unfortunately, there is only a table for low-power mode,
 * and the actual cutoff-frequency depends on the ODR.
 * Naming scheme after ODR@400Hz
 * AN3308 > section 4.3.1.1 */
#define LIS3DH_FILTER_CUTOFF_8 0x00 /* highest freq */
#define LIS3DH_FILTER_CUTOFF_4 0x01
#define LIS3DH_FILTER_CUTOFF_2 0x02
#define LIS3DH_FILTER_CUTOFF_1 0x03 /* lowest freq */

/* Positive and negative axis self-test */
#define LIS3DH_SELF_TEST_0      0x01
#define LIS3DH_SELF_TEST_1      0x02


/* Note: IA{1,2} is interrupt activity {1,2} or interrupt generators */
/* user provided functions, init and deinit can be set to NULL and won't be used */
struct lis3dh_device {
    int (*init)(void);
    int (*read)(uint8_t reg, uint8_t *dst, uint32_t size);
    int (*write)(uint8_t reg, uint8_t value);
    int (*sleep)(uint32_t dur_us);
    int (*deinit)(void);
};

struct lis3dh_click_config {
    uint8_t zd; /* double click interrupt on Z-axis */
    uint8_t zs; /* single click interrupt on Z-axis */
    uint8_t yd; /* double click interrupt on Y-axis */
    uint8_t ys; /* single click interrupt on Y-axis */
    uint8_t xd; /* double click interrupt on X-axis */
    uint8_t xs; /* single click interrupt on X-axis */
    uint8_t latch; /* active until CLICK_SRC is read */
};

/* INT1_CFG and INT2_CFG have identical struct */
struct lis3dh_int_config {
    uint8_t aoi; /* AND/OR combination of int events */
    uint8_t en_6d; /* 6 direction detection */
    uint8_t en_4d; /* both en_6d and en_4d must = 1 for 4D to work ! */
    uint8_t zh; /* interrupt generation on Z high event / Dir. recog. */
    uint8_t zl; /* interrupt generation on Z low event / Dir. recog. */
    uint8_t yh; /* interrupt generation on Y high event / Dir. recog. */
    uint8_t yl; /* interrupt generation on Y low event / Dir. recog. */
    uint8_t xh; /* interrupt generation on X high event / Dir. recog. */
    uint8_t xl; /* interrupt generation on X low event / Dir. recog. */
    uint8_t latch; /* active until INT1_SRC/INT2_SRC is read */
};

/* config for interrupt output pin #2 */
struct lis3dh_int_pin2_config {
    uint8_t click; /* CLICK interrupt */
    uint8_t ia1; /* IA1 interrupt */
    uint8_t ia2; /* IA2 interrupt */
    uint8_t boot; /* enable BOOT on pin 2 */
    uint8_t act; /* interrupt on activity */
    uint8_t polarity; /* INT1 & INT2 polarity. 0 active high, 1 active low */
};

/* config for interrupt output pin #1 */
struct lis3dh_int_pin1_config {
    uint8_t click; /* CLICK interrupt */
    uint8_t ia1; /* IA1 interrupt */
    uint8_t ia2; /* IA2 interrupt */
    uint8_t drdy_zyxda; /* new [xyz] data ready (not via FIFO) */
    uint8_t drdy_321; /* not sure */
    uint8_t wtm; /* FIFO reached watermark level */
    uint8_t overrun; /* FIFO has overrun */
};

/* config for high-pass filter */
struct lis3dh_filter_config {
    uint8_t mode; /* filter mode, reset behaviour */
    uint8_t cutoff; /* high-pass filter cutoff freq (~ ODR) */
    uint8_t fds; /* ¬(bypass filter) */
    uint8_t click; /* enable filter for CLICK function */
    uint8_t ia2; /* enable filter for AOI func on INT 2 */
    uint8_t ia1; /* enable filter for AOI func on INT 1 */
};

/* config for FIFO */
struct lis3dh_fifo_config {
    uint8_t size; /* size of fifo; 0-32; 0=don't use. fth=[1-32]-1 (index) */
    uint8_t trig; /* pin to trigger when watermark/overrun occurs */
    uint8_t mode; /* FIFO mode */
};

struct lis3dh_config {
    uint8_t rate; /* ODR */
    uint8_t range; /* FS */
    uint8_t mode; /* LPen and HR */
    struct lis3dh_fifo_config     fifo;
    struct lis3dh_filter_config   filter;
    struct lis3dh_int_pin1_config pin1;
    struct lis3dh_int_pin2_config pin2;
    struct lis3dh_int_config      int1;
    struct lis3dh_int_config      int2;
    struct lis3dh_click_config    click;

    /* 1 LSb = 16 mg @ FS_2G 
     * 1 LSb = 32 mg @ FS_4G
     * 1 LSb = 62 mg @ FS_8G
     * 1 LSb = 186 mg @ FS_16G 
     */
    uint8_t int1_ths; /* 7-bit INT 1 threshold value */
    uint8_t int2_ths; /* 7-bit INT 2 threshold value */
    uint8_t click_ths; /* 7-bit CLICK threshold value */
    uint8_t act_ths; /* 7-bit ACT threshold value */

    /* Duration time is measured in N/ODR where:
     * --- N = The content of the intX_dur integer
     * --- ODR = the data rate, eg 100, 400...
     * [ODR] [1 LSb in milliseconds]
     *   1      1000
     *   10     100
     *   25     40
     *   50     20
     *   100    10
     *   200    5
     *   400    2.5
     *   1600   0.6
     *   1344   0.744
     *   5376   0.186
     * (Note: this calc also applies to:
     *      - time_limit
     *      - time_window
     *      - time_latency
     * used in CLICK)
     * 
     * this is the "1[LSb]" in act_dur calc
     */
    uint8_t int1_dur; /* 7-bit INT 1 duration value */
    uint8_t int2_dur; /* 7-bit INT 2 duration value */

    /* Sleep-to-wake and return-to-sleep duration 
     * 1 LSb = (8 * 1[LSb] + 1) / ODR
     */
    uint8_t act_dur; /* 8-bit ACT duration value */

    uint8_t time_limit; /* 7-bit time limit ~ CLICK */
    uint8_t time_latency; /* 8-bit time latency ~ CLICK */
    uint8_t time_window; /* 8-bit time window ~ CLICK */

    uint8_t sdo_pullup; /* Use pull-up on SDO. default 0 use */
    uint8_t en_adc; /* enable ADC */
    uint8_t en_temp; /* enable temp sensor on ADC 3 */
    uint8_t spi3w; /* set to 1 to enable 3-way SPI, default 0 */
    uint8_t self_test; /* set self-test mode */

    uint8_t reference; /* HP filter reference */
};

/* reads from internal ADCs. 
 * Input range: 800 mV to 1600 mV 
 * Resolution:
 *              8-bit in LP mode
 *             10-bit in normal and in HR mode.
 * Sampling frequency:
 *              same as ODR
 * Output:
 *            actual value in mV
 *            1 lsb = 1 mv
 * Temperature:
 *            1 lsb = 1 deg C
 */
struct lis3dh_adc {
    int16_t adc1;
    int16_t adc2;
    int16_t adc3;
};

/* accel data read not using FIFO */
/* 1 lsb = 1 mg */
struct lis3dh_accel {
    int16_t x;
    int16_t y;
    int16_t z;
};

/* stores interrupt source registers read from the device */
struct lis3dh_int_src {
    uint8_t int1;
    uint8_t int2;
    uint8_t click;
};

struct lis3dh {
    struct lis3dh_device  dev; /* fn ptrs to interface w/ device */
    struct lis3dh_config  cfg; /* config variables to write to device */
    struct lis3dh_accel   acc; /* accel data read from device (not FIFO) */
    struct lis3dh_adc     adc; /* adc and optionally temp read from device */
    struct lis3dh_int_src src; /* INT_SRC registers read from device */
};

typedef struct lis3dh lis3dh_t;

/* struct for containing the FIFO data */
/* 1 lsb = 1 mg */
struct lis3dh_fifo_data {
    uint8_t size; /* up to 31 */
    int16_t x[32];
    int16_t y[32];
    int16_t z[32];
};

int lis3dh_init(lis3dh_t *lis3dh);
int lis3dh_deinit(lis3dh_t *lis3dh);
int lis3dh_configure(lis3dh_t *lis3dh);
int lis3dh_read(lis3dh_t *lis3dh);
int lis3dh_read_fifo(lis3dh_t *lis3dh, struct lis3dh_fifo_data *fifo);
int lis3dh_read_int1(lis3dh_t *lis3dh);
int lis3dh_read_int2(lis3dh_t *lis3dh);
int lis3dh_read_click(lis3dh_t *lis3dh);
int lis3dh_reference(lis3dh_t *lis3dh);
int lis3dh_reset(lis3dh_t *lis3dh);
int lis3dh_read_adc(lis3dh_t *lis3dh);
int lis3dh_read_temp(lis3dh_t *lis3dh);
int lis3dh_fifo_reset(lis3dh_t *lis3dh);

#endif