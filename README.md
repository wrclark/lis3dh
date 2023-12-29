# LIS3DH

A C89 driver for the 3-axis accelerometer LIS3DH. Supports both i2c and SPI. 

> ### Features
> - FIFO of varying watermark level, up to 32
> - HP filter (4 c/o freq)
> - 2G, 4G, 8G and 16G
> - All power modes
> - Interrupt generation (partial)
> - Free-fall detection (soon)
> - Single and double click detection (soon)
> - 4D/6D orientation detection (soon)

## Implementation
This driver requires the user to provide pointers to the following abstractely named functions:

This project has example interface code for I2C used on Raspberry Pi.
```c
/* initialise the "interface" */
int init(void);
/* read from device register `reg`, `size` amount of bytes and write them to `dst` */
int read(uint8_t reg, uint8_t *dst, uint32_t size);
/* write `value` to device register `reg` */
int write(uint8_t reg, uint8_t value);
/* sleep for `dur_us` microseconds */
int sleep(uint32_t dur_us);
/* deinitalise the "interface" */
int deinit(void);
```
All above functions return `0` on success.

The `init` and `deinit` pointers can both be set to `NULL` and they won't be run.

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
...
```
## Using FIFO
Instead of polling for every single [x y z] set, a FIFO with programmable capacity ("watermark") can be used like such:

All FIFO readings use 10-bit resolution regardless of the mode set in `lis.cfg.mode`.

The watermark level can also be adjusted to a value [0-31] inclusive by modifying the `lis.cfg.fifo.fth` property before calling configure().
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
...
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
...
```
## Using interrupts

The LIS3DH supports two different interrupt "output pins," `INT1` and `INT2`. The appropriate flag must be set in either `cfg.int1` or `cfg.int2` (only one of such flags can be set at a time!) and the interrupt source must be configured to trigger into `INT1` or `INT2`. Below is example code that listens and receives an interrupt when the FIFO watermark is reached i.e. it is full.

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "lis3dh.h"
#include "interrupt.h"
#include "i2c.h"

/* GPIO 12 or Pin 32 */
#define GPIO_INTERRUPT_PIN 12

int main() {

    lis3dh_t lis;
    struct lis3dh_fifo_data fifo;
    int k;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;

    if (lis3dh_init(&lis)) {
        /* error handling */
    }

    /* register interrupt */
    if (int_register(GPIO_INTERRUPT_PIN)) {
        /* error handling */
    }

    lis.cfg.mode = LIS3DH_MODE_NORMAL;
    lis.cfg.range = LIS3DH_FS_2G;
    lis.cfg.rate = LIS3DH_ODR_100_HZ;
    lis.cfg.fifo.mode = LIS3DH_FIFO_MODE_STREAM;
    lis.cfg.fifo.trig = LIS3DH_FIFO_TRIG_INT1; /* trigger into INT1 */
    lis.cfg.int1.wtm = 1; /* trigger upon watermark level reached */
    
    if (lis3dh_configure(&lis)) {
       /* error handling */
    }
    
    /* wait for interrupt from LIS3DH */
    if (int_poll(GPIO_INTERRUPT_PIN)) {
        /* error handling */
    }

    if (lis3dh_read_fifo(&lis, &fifo)) {
       /* error handling */
    }

    for(k=0; k<fifo.size; k++) {
        printf("%04.04f %04.04f %04.04f %04.04f\n", fifo.x[k], fifo.y[k], fifo.z[k]);
    }
    
    /* unregister interrupt */
    if (int_unregister(GPIO_INTERRUPT_PIN)) {
        /* error handling */
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
0.2040 -1.0120 -0.1720 
0.2200 -1.0200 -0.1600 
0.2160 -1.0200 -0.1600
...
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

