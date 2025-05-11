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


/* 

Example SPI use on linux/raspberry pi

*/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>

#include "spi.h"

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED 1000 * 1000 /* 1 MHz */

/*
 * Pinout config for this example
 *
 * MOSI - GPIO 10 (physical pin 19) => LIS3DH "SDA" or "SDI"
 * MISO - GPIO 9  (physical pin 21) => LIS3DH "SDO"
 * SCLK - GPIO 11 (physical pin 23) => LIS3DH "SCL"
 * CE0  - GPIO 8  (physical pin 24) => LIS3DH "!CS"
 *
 */

static int fd;

int spi_init(void) {
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED;

    if ((fd = open(SPI_DEVICE, O_RDWR)) < 0) {
        fprintf(stderr, "spi_init(): open(%s)\n", SPI_DEVICE);
        return 1;
    }

    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1) {
        fprintf(stderr, "spi_init(): SPI_IOC_RD_MODE\n");
        return 1;
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) {
        fprintf(stderr, "spi_init(): SPI_IOC_WR_MODE\n");
        return 1;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        fprintf(stderr, "spi_init(): SPI_IOC_WR_BITS_PER_WORD\n");
        return 1;
    }

    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1) {
        fprintf(stderr, "spi_init(): SPI_IOC_RD_BITS_PER_WORD\n");
        return 1;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        fprintf(stderr, "spi_init(): SPI_IOC_WR_MAX_SPEED_HZ\n");
        return 1;
    }

    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
        fprintf(stderr, "spi_init(): SPI_IOC_RD_MAX_SPEED_HZ\n");
        return 1;
    }
    
    return 0;
}

int spi_read(uint8_t reg, uint8_t *dst, uint32_t size) {

    uint8_t send[2];
    struct spi_ioc_transfer tr[2] = {0};

    /* clear 2 MSbits */
    reg &= 0x3F;

    /* set READ bit */
    reg |= 0x80;
    
    if (size > 1) {
        /* set AUTO INC bit */
        reg |= 0x40;
    }

    send[0] = reg;
    send[1] = 0x00;

    tr[0].tx_buf = (unsigned long) send;
    tr[0].rx_buf = (unsigned long) 0;
    tr[0].len = 2;

    tr[1].tx_buf = (unsigned long) 0;
    tr[1].rx_buf = (unsigned long) dst;
    tr[1].len = size;

    if (ioctl(fd, SPI_IOC_MESSAGE(2), tr) < 0) {
        fprintf(stderr, "spi_read(): error ioctl()\n");
        return 1;
    }

    return 0;
}

int spi_write(uint8_t reg, uint8_t value) {
    struct spi_ioc_transfer tr[2] = {0};

    /* clear 2 MSbits */
    reg &= 0x3F;

    tr[0].tx_buf = (unsigned long) &reg;
    tr[0].rx_buf = (unsigned long) 0;
    tr[0].len = 1;

    tr[1].tx_buf = (unsigned long) &value;
    tr[1].rx_buf = (unsigned long) 0;
    tr[1].len = 1;

    if (ioctl(fd, SPI_IOC_MESSAGE(2), tr) < 0) {
        fprintf(stderr, "spi_write(): error ioctl()\n");
        return 1;
    }

    return 0;
}

int spi_deinit(void) {

    if (fd) {
        close(fd);
    }

    return 0;
}

