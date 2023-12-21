#ifndef I2C_H
#define I2C_H

#include <stdint.h>

int i2c_init (void);
int i2c_read (uint8_t reg, uint8_t *dst, uint32_t size);
int i2c_write (uint8_t reg, uint8_t value);
int i2c_deinit (void);

#endif