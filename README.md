# LIS3DH

A C89 driver for the 3-axis accelerometer LIS3DH. Supports both i2c and SPI. 

> ### Features
> - FIFO of varying watermark level, up to 32
> - HP filter (4 c/o freq)
> - 2G, 4G, 8G and 16G
> - All power modes
> - Interrupt generation (soon)
> - Free-fall detection (soon)
> - Single and double click detection (soon)
> - 4D/6D orientation detection (soon)

> ### Notes
> FIFO is always and only 10-bit


## Usage
Simple example:
```c
#define _GNU_SOURCE
#include <unistd.h> /* usleep() */
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {
    
    lis3dh_t lis;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    if (lis3dh_poll(&lis)) {
        /* error handling */
    }

    if (lis3dh_read(&lis)) {
        /* error handling */
    }

    printf("x: %f, y: %f, z: %f\n", lis.acc.x, lis.acc.y, lis.acc.z);

    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}
```
The output should be something similar to this:
```
$ ./lis3dh 
x: 0.540000, y: -0.882000, z: -0.100000
```
poll() and read() can be indefinitely looped for a constant data stream, like this:
```c
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;

    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    for(i=0; i<10; i++) {

        if (lis3dh_poll(&lis)) {
            /* error handling */
        }

        if (lis3dh_read(&lis)) {
            /* error handling */
        }

        printf("x: %f, y: %f, z: %f\n", lis.acc.x, lis.acc.y, lis.acc.z);
    }

    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}
```
Output:
```
$ ./lis3dh 
x: 0.534000, y: -0.882000, z: -0.102000
x: 0.538000, y: -0.866000, z: -0.136000
x: 0.518000, y: -0.846000, z: -0.100000
x: 0.518000, y: -0.840000, z: -0.098000
x: 0.542000, y: -0.876000, z: -0.098000
x: 0.518000, y: -0.834000, z: -0.146000
x: 0.512000, y: -0.854000, z: -0.106000
x: 0.574000, y: -0.870000, z: -0.122000
x: 0.518000, y: -0.846000, z: -0.098000
x: 0.516000, y: -0.852000, z: -0.112000
```
## Using FIFO
Instead of polling for every single [x y z] set, a FIFO with programmable capacity ("watermark") can be used like such:

It should be noted that all FIFO readings use 10-bit resolution regardless of the mode. The watermark level can also be adjusted to a value [0-32] inclusive by modifying the `lis.cfg.fifo.fth` property before calling configure().
```c
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data data;
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_NORMAL;

    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    if (lis3dh_poll_fifo(&lis)) {
        /* error handling */
    }

    if (lis3dh_read_fifo(&lis, &data)) {
        /* error handling */
    }

    /* read out fifo buffer data */
    for(i=0; i<data.size; i++) {
        printf("x: %f, y: %f, z: %f\n", data.x[i], data.y[i], data.z[i]);
    }

    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}
```
Output:
```
$ ./lis3dh 
x: 0.544000, y: -0.904000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.528000, y: -0.880000, z: -0.104000
x: 0.528000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.104000
x: 0.536000, y: -0.880000, z: -0.128000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.568000, y: -0.872000, z: -0.160000
x: 0.560000, y: -0.872000, z: -0.160000
```
## Using the HP filter
The LIS3DH can optionally apply a HP filter on the sample data. It can be used to greatly reduce the "DC acceleration" present.
```c
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data data;
    int i;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_HR;
    lis.cfg.range = LIS3DH_FS_4G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_NORMAL;
    lis.cfg.filter.mode = LIS3DH_FILTER_MODE_AUTORESET;
    lis.cfg.filter.cutoff = LIS3DH_FILTER_CUTOFF_8;

    if (lis3dh_configure(&lis)) {
        /* error handling */
    }

    if (lis3dh_poll_fifo(&lis)) {
        /* error handling */
    }

    if (lis3dh_read_fifo(&lis, &data)) {
        /* error handling */
    }

    /* read out fifo buffer data */
    for(i=0; i<data.size; i++) {
        printf("x: %f, y: %f, z: %f\n", data.x[i], data.y[i], data.z[i]);
    }

    if (lis3dh_deinit(&lis)) {
        /* error handling */
    }

    return 0;
}
```
Output:
```
$ ./lis3dh 
x: 0.008000, y: 0.000000, z: 0.000000
x: 0.000000, y: 0.000000, z: 0.016000
x: -0.016000, y: -0.008000, z: -0.008000
x: -0.032000, y: 0.008000, z: 0.000000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.032000
x: 0.024000, y: -0.016000, z: -0.008000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.016000, y: -0.032000, z: -0.040000
x: -0.008000, y: -0.024000, z: -0.008000
```

### Using i2c on STM32
Simple example code
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

