#ifndef SPI_H
#define SPI_H

#include <stdint.h>

int spi_init(void);
int spi_read(uint8_t reg, uint8_t *dst, uint32_t size);
int spi_write(uint8_t reg, uint8_t value);
int spi_deinit(void);

#endif