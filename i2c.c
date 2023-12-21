/* 

Example I2C use on linux/raspberry pi

*/

#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include "i2c.h"

#define I2C_DEVICE "/dev/i2c-1"
#define I2C_LIS3DH_ADDRESS 0x18

static int fd;

int i2c_init(void) {
	fd = open(I2C_DEVICE, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "could not open device: %s\n", I2C_DEVICE);
		return I2C_ERR;
	}

	if (ioctl(fd, I2C_SLAVE, I2C_LIS3DH_ADDRESS) < 0) {
		fprintf(stderr, "failed to acquire bus/talk to slave\n");
		close(fd);
		return I2C_ERR;
	}

	return I2C_OK;
}


int i2c_read(uint8_t reg, uint8_t *dst, uint32_t size) {
	uint8_t cmd[2] = {reg, 0x00};
	write(fd, cmd, 2);
	
	if (read(fd, dst, size) != (ssize_t)size) {
		fprintf(stderr, "error read()\n");
		return I2C_ERR;
	
	}

	return I2C_OK;
}


int i2c_write(uint8_t reg, uint8_t value) {
	uint8_t cmd[2] = {reg, value};
	
	if (write(fd, cmd, 2) != 2) {
		fprintf(stderr, "error write()\n");
		return I2C_ERR;
	}

	return I2C_OK;
}

int i2c_deinit(void) {
	if (fd) {
		close(fd);
	}

	return I2C_OK;
}
