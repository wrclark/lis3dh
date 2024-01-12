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
> - Sleep-to-Wake/Return-to-Sleep
> - Self test


## Examples
See the `example/` dir for complete code examples and explanations of LIS3DH terminology

## Implementation
This driver requires the user to implement the following interface functions:

This project has example interface code for I2C and SPI used on Raspberry Pi 4 and STM32
```c
/* initialise the "interface" */
int init(void);
/* read from register `reg', `size' amount of bytes, and write them to `dst' */
int read(uint8_t reg, uint8_t *dst, uint32_t size);
/* write `value' to register `reg' */
int write(uint8_t reg, uint8_t value);
/* sleep for `dur_us' microseconds */
int sleep(uint32_t dur_us);
/* deinitalise the "interface" */
int deinit(void);
```
All above functions return `0` on success, and any non-zero value on error.

If `init` and/or `deinit` are set to `NULL`, they will be ignored. Useful on microcontrollers.

---
## Functions

```c
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
```
All functions return `0` on success, and any non-zero value on error.

## STM32
### I2C
```c
#define LIS3DH_I2C_ADDR 0x18 /* can also be 0x19 */

int i2c_write(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = { reg, value };
    HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_I2C_ADDR << 1, buf, 2, HAL_MAX_DELAY);
    return 0;
}

int i2c_read(uint8_t reg, uint8_t *dst, uint32_t size) {
    if (size > 1) {
        reg |= 0x80; /* auto-increment bit */
    }
    uint8_t send[2] = { reg, 0x00 };
    HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_I2C_ADDR << 1, send, 2, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c2, LIS3DH_I2C_ADDR << 1, dst, size, HAL_MAX_DELAY);
    return 0;
}
```

### SPI
CPHA=0 CPOL=0 8 bits motorola
```c
/* Every sleep call duration is a multiple of 1000 us (1 ms) */
/* So dividing this value by 1000 is perfectly fine. */
int sleep_us(uint32_t dur_us) {
	HAL_Delay(dur_us / 1000);
	return 0;
}

int spi_write(uint8_t reg, uint8_t value) {
	uint8_t send[2];

	reg &= 0x3F; /* clear 2 msbit */
	send[0] = reg;
	send[1] = value;

	/* CS LOW */
	HAL_GPIO_WritePin(SPI1_GPIO_CS_GPIO_Port, SPI1_GPIO_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, send, 2, HAL_MAX_DELAY);
	/* CS HIGH */
	HAL_GPIO_WritePin(SPI1_GPIO_CS_GPIO_Port, SPI1_GPIO_CS_Pin, GPIO_PIN_SET);

	return 0;
}

int spi_read(uint8_t reg, uint8_t *dst, uint32_t size) {
    uint8_t send[2];
	reg |= 0x80; /* read bit = 1 */

	if (size > 1) {
		reg |= 0x40; /* auto increment for rx > 1 */
	}

	send[0] = reg;
	send[1] = 0x00;

	/* CS LOW */
	HAL_GPIO_WritePin(SPI1_GPIO_CS_GPIO_Port, SPI1_GPIO_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, send, 2, 1000);
    HAL_SPI_Receive(&hspi1, dst, size, 1000);
    /* CS HIGH */
    HAL_GPIO_WritePin(SPI1_GPIO_CS_GPIO_Port, SPI1_GPIO_CS_Pin, GPIO_PIN_SET);

	return 0;
}
```