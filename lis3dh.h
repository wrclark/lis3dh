#ifndef LIS3DH_H
#define LIS3DH_H

#include <stdint.h>

/* rates */
#define LIS3DH_ODR_POWEROFF 0b0000
#define LIS3DH_ODR_1_HZ  0b0001
#define LIS3DH_ODR_10_HZ 0b0010
#define LIS3DH_ODR_25_HZ 0b0011
#define LIS3DH_ODR_50_HZ 0b0100
#define LIS3DH_ODR_100_HZ 0b0101
#define LIS3DH_ODR_200_HZ 0b0110
#define LIS3DH_ODR_400_HZ 0b0111
#define LIS3DH_ODR_NORM_1344_HZ 0b1001
#define LIS3DH_ODR_LP_1600_HZ 0b1000
#define LIS3DH_ODR_LP_5376_HZ 0b1001

/* range/sens */
#define LIS3DH_FS_2G 0b00
#define LIS3DH_FS_4G 0b01
#define LIS3DH_FS_8G 0b10
#define LIS3DH_FS_16G 0b11

/* operating modes */
#define LIS3DH_MODE_HR 0b00
#define LIS3DH_MODE_LP 0b01
#define LIS3DH_MODE_NORMAL 0b10

/* FIFO modes */
#define LIS3DH_FIFO_MODE_BYPASS 0b00
#define LIS3DH_FIFO_MODE_NORMAL 0b01 /* "FIFO" */
#define LIS3DH_FIFO_MODE_STREAM 0b10
#define LIS3DH_FIFO_MODE_STREAM_TO_FIFO 0b11

/* FIFO trigger pin selection */
#define LIS3DH_FIFO_TRIG_INT1 0b0
#define LIS3DH_FIFO_TRIG_INT2 0b1

struct lis3dh_device {
	int (*init)(void);
	int (*read)(uint8_t reg, uint8_t *dst, uint32_t size);
	int (*write)(uint8_t reg, uint8_t value);
	int (*sleep)(uint32_t dur_us);
	int (*deinit)(void);
};

struct lis3dh_fifo_config {
    uint8_t fth;
    uint8_t trig;
    uint8_t mode;
};

struct lis3dh_config {
    uint8_t rate; /* ODR */
    uint8_t range; /* FS */
    uint8_t mode; /* LPen and HR */
    struct lis3dh_fifo_config fifo;
};

struct lis3dh_acceleration {
    double x;
    double y;
    double z;
};

struct lis3dh {
    struct lis3dh_device dev;
    struct lis3dh_config cfg;
    struct lis3dh_acceleration acc;
};

typedef struct lis3dh lis3dh_t;

int lis3dh_init(lis3dh_t *lis3dh);
int lis3dh_deinit(lis3dh_t *lis3dh);
int lis3dh_configure(lis3dh_t *lis3dh);
int lis3dh_poll(lis3dh_t *lis3dh);
int lis3dh_read(lis3dh_t *lis3dh);
int lis3dh_poll_fifo(lis3dh_t *lis3dh);
int lis3dh_read_fifo(lis3dh_t *lis3dh);

#endif