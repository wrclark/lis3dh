# LIS3DH

A C89 driver for the 3-axis accelerometer LIS3DH. Supports both I2C and SPI. 

### Features
> - FIFO 
> - HP filter
> - 2G, 4G, 8G and 16G
> - Low-power mode, normal mode and high-resolution mode
> - ADC and temperature sensing
> - Interrupt generation
> - Free-fall detection
> - Single-click detection
> - Double-click detection
> - 4D/6D orientation detection


## Examples
See the `examples/` dir for complete code examples

## Implementation
This driver requires the user to implement the following interface functions:

This project has example interface code for I2C and SPI (broken) used on Raspberry Pi 4.
```c
/* initialise the "interface" */
int init(void);
/* read from register `reg`, `size` amount of bytes, and write them to `dst` */
int read(uint8_t reg, uint8_t *dst, uint32_t size);
/* write `value` to register `reg` */
int write(uint8_t reg, uint8_t value);
/* sleep for `dur_us` microseconds */
int sleep(uint32_t dur_us);
/* deinitalise the "interface" */
int deinit(void);
```
All above functions return `0` on success, and any non-zero value on error.

If `init` and/or `deinit` are set to `NULL`, they will be ignored. Useful on microcontrollers.

---

### Using i2c on STM32
Example code because I couldn't previously find working examples.
```c
#define LIS3DH_I2C_ADDR 0x18

int i2c_write(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = { reg, value };
    HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_I2C_ADDR << 1, buf, 2, HAL_MAX_DELAY);
    return 0;
}

int i2c_read(uint8_t reg, uint8_t *dst, uint32_t size) {
    uint8_t send[2] = { reg, 0x00 };
    HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_I2C_ADDR << 1, send, 2, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c2, LIS3DH_I2C_ADDR << 1, dst, size, HAL_MAX_DELAY);
    return 0;
}
```

