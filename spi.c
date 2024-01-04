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


/*
 * Pinout config for this example
 *
 * MOSI - GPIO 10 (physical pin 19) => LIS3DH "SDA" or "SDI"
 * MISO - GPIO 9  (physical pin 21) => LIS3DH "SDO"
 * SCLK - GPIO 11 (physical pin 23) => LIS3DH "SCK"
 * CE0  - GPIO 8  (physical pin 24) => LIS3DH "!CS"
 * 
 * Broken for unknown reason on Pi 4
 *
 */

#define SPI_SPEED 500 * 1000 /* 500 KHz */

static int fd;
static uint8_t rx[256];
static uint8_t tx[256];

int spi_init(void) {
	uint8_t mode = SPI_MODE_3;
	uint8_t bits = 8;
	uint32_t speed = SPI_SPEED;

	if ((fd = open(SPI_DEVICE, O_RDWR)) < 0) {
		fprintf(stderr, "spi open(%s)\n", SPI_DEVICE);
		return 1;
	}

	if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1) {
		fprintf(stderr, "SPI_IOC_RD_MODE\n");
		return 1;
	}

	if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) {
		fprintf(stderr, "SPI_IOC_WR_MODE\n");
		return 1;
	}

	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
		fprintf(stderr, "SPI_IOC_WR_BITS_PER_WORD\n");
		return 1;
	}

	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1) {
		fprintf(stderr, "SPI_IOC_RD_BITS_PER_WORD\n");
		return 1;
	}

	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
		fprintf(stderr, "SPI_IOC_WR_MAX_SPEED_HZ\n");
		return 1;
	}

	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
		fprintf(stderr, "SPI_IOC_RD_MAX_SPEED_HZ\n");
		return 1;
	}
	
	return 0;
}

static int spi_transaction(uint8_t *tx, uint8_t *rx, uint32_t size) {
	struct spi_ioc_transfer tr = {0};

	tr.tx_buf = (unsigned long) tx;
	tr.rx_buf = (unsigned long) rx;
	tr.len = size;

	return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

int spi_read(uint8_t reg, uint8_t *dst, uint32_t size) {

	/* clear 2 MSbits */
	reg &= 0x3F;

	/* set READ bit */
    reg |= 0x80;
    
    if (size > 1) {
		/* set AUTO INC bit */
        reg |= 0x40;
    }

	tx[0] = reg;

	if (spi_transaction(tx, rx, size + 1) < 0) {
		fprintf(stderr, "spi_read()\n");
		return 1;
	}

	memcpy(dst, rx + 1, size);

	return 0;
}

int spi_write(uint8_t reg, uint8_t value) {

	/* clear 2 MSbits */
	reg &= 0x3F;

	tx[0] = reg;
	tx[1] = value;

	if (spi_transaction(tx, rx, 2) < 0) {
		fprintf(stderr, "spi_write()\n");
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

