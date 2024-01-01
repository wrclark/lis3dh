# lis3dh/example

### simple.c
Basic example of how to use this device

### fifo.c
Instead of polling for every single [x y z] set, a FIFO with programmable capacity ("watermark") can be used, and then dumped into memory once full.

All FIFO readings use 10-bit resolution regardless of the mode set in `lis.cfg.mode`.

The watermark level can be adjusted to a value [0-31] by modifying the `lis.cfg.fifo.fth` property before calling `lis3dh_configure()`.

The LIS3DH can optionally apply a HP filter on the sample data. It can be used to greatly reduce the "DC acceleration" present.

### interrupts.c
This device supports two different interrupt "output pins," `INT1` and `INT2`. The appropriate flag must be set in either `cfg.pin1` or `cfg.pin2` and the interrupt source must be configured to trigger into `INT1` or `INT2`. This file contains example code that listens and receives an interrupt when the FIFO watermark is reached i.e. it is full.

### single-click.c

Set up single-click detection

### double-click.c

Set up double-click detection

### adc.c 

Enable and read built-in ADCs.

> - Input range: 800 mV to 1600 mV
> - Resolution: 8-bit in LP mode, 10-bit in normal and in HR mode.
> - Sampling frequency: same as ODR

### temp.c

Enable and read built-in temperature sensor

> - Operating range: -40 to 85°C
> - Step size: ±1°C