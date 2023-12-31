#ifndef LIS3DH_H
#define LIS3DH_H

#include <stdint.h>

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
#define LIS3DH_FS_2G 0x00
#define LIS3DH_FS_4G 0x01
#define LIS3DH_FS_8G 0x02
#define LIS3DH_FS_16G 0x03

/* operating modes */
#define LIS3DH_MODE_HR 0x00
#define LIS3DH_MODE_LP 0x01
#define LIS3DH_MODE_NORMAL 0x02

/* FIFO modes */
#define LIS3DH_FIFO_MODE_BYPASS 0x00
#define LIS3DH_FIFO_MODE_NORMAL 0x01 /* "FIFO" */
#define LIS3DH_FIFO_MODE_STREAM 0x02
#define LIS3DH_FIFO_MODE_STREAM_TO_FIFO 0x03

/* FIFO trigger pin selection */
#define LIS3DH_FIFO_TRIG_INT1 0x00
#define LIS3DH_FIFO_TRIG_INT2 0x01

/* filter modes */
/* this one is reset by reading REFERENCE (0x26) */
#define LIS3DH_FILTER_MODE_NORMAL 0x00
#define LIS3DH_FILTER_MODE_REFERENCE 0x01
#define LIS3DH_FILTER_MODE_NORMAL2 0x02 /* same as 00? */
#define LIS3DH_FILTER_MODE_AUTORESET 0x03

/* filter cutoff */
/* unfortunately, there is only a table for low-power mode,
   and the actual cutoff-frequency depends on the ODR.
   Naming scheme after ODR@400Hz
   AN3308 > section 4.3.1.1 */
#define LIS3DH_FILTER_CUTOFF_8 0x00 /* highest freq */
#define LIS3DH_FILTER_CUTOFF_4 0x01
#define LIS3DH_FILTER_CUTOFF_2 0x02
#define LIS3DH_FILTER_CUTOFF_1 0x03 /* lowest freq */


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
};

/* config for INT2 trigger output */
struct lis3dh_int_pin2_config {
    uint8_t click; /* CLICK interrupt */
    uint8_t ia1; /* IA1 interrupt */
    uint8_t ia2; /* IA2 interrupt */
    uint8_t boot; /* enable BOOT on pin 2 */
    uint8_t act; /* interrupt on activity */
    uint8_t polarity; /* INT1 & INT2 polarity. 0 active high, 1 active low */
    uint8_t latch;  /* active until INT2_SRC read (reg5:1) */
};

/* config for INT1 trigger output */
struct lis3dh_int_pin1_config {
    uint8_t click; /* CLICK interrupt */
    uint8_t ia1; /* IA1 interrupt */
    uint8_t ia2; /* IA2 interrupt */
    uint8_t drdy_zyxda; /* new [xyz] data ready (not via FIFO) */
    uint8_t drdy_321; /* not sure */
    uint8_t wtm; /* FIFO reached watermark level */
    uint8_t overrun; /* FIFO has overrun */
    uint8_t latch;  /* active until INT1_SRC read (reg5:0) */
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
    uint8_t fth; /* user-specified watermark level 0-32 */
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
 */
struct lis3dh_adc {
    float adc1;
    float adc2;
    float adc3;
};

/* accel data not read from FIFO is put here */
struct lis3dh_accel {
    float x;
    float y;
    float z;
};

struct lis3dh {
    struct lis3dh_device dev;
    struct lis3dh_config cfg;
    struct lis3dh_accel  acc;
    struct lis3dh_adc    adc;
};

typedef struct lis3dh lis3dh_t;

/* struct for containing the FIFO data */
struct lis3dh_fifo_data {
    uint8_t size; /* up to 32 */
    float x[32];
    float y[32];
    float z[32];
};

int lis3dh_init(lis3dh_t *lis3dh);
int lis3dh_deinit(lis3dh_t *lis3dh);
int lis3dh_configure(lis3dh_t *lis3dh);
int lis3dh_poll(lis3dh_t *lis3dh);
int lis3dh_read(lis3dh_t *lis3dh);
int lis3dh_poll_fifo(lis3dh_t *lis3dh);
int lis3dh_read_fifo(lis3dh_t *lis3dh, struct lis3dh_fifo_data *fifo);
int lis3dh_clear_int1(lis3dh_t *lis3dh);
int lis3dh_clear_int2(lis3dh_t *lis3dh);
int lis3dh_reference(lis3dh_t *lis3dh);
int lis3dh_reset(lis3dh_t *lis3dh);
int lis3dh_read_adc(lis3dh_t *lis3dh);
int lis3dh_read_temp(lis3dh_t *lis3dh);

#endif