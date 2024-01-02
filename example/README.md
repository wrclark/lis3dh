# lis3dh/example

### file: simple.c
Basic example of how to use this device

### file: fifo.c
Instead of polling for every single [x y z] set, a FIFO with programmable capacity ("watermark") can be used, and then dumped into memory once full.

All FIFO readings use 10-bit resolution regardless of the mode set in `lis.cfg.mode`.

The watermark level can be adjusted to a value [0-31] by modifying the `lis.cfg.fifo.fth` property before calling `lis3dh_configure()`.

The LIS3DH can optionally apply a HP filter on the sample data. It can be used to greatly reduce the "DC acceleration" present.

### file: interrupts.c
This device supports two different interrupt "output pins," `INT1` and `INT2`. The appropriate flag must be set in either `cfg.pin1` or `cfg.pin2` and the interrupt source must be configured to trigger into `INT1` or `INT2`. This file contains example code that listens and receives an interrupt when the FIFO watermark is reached i.e. it is full.

### file: single-click.c

Set up single-click detection (no latching interrupt possible)

### file: double-click.c

Set up double-click detection (no latching interrupt possible)

### file: adc.c 

Enable and read built-in ADCs.

> - Input range: 800 mV to 1600 mV
> - Resolution: 8-bit in LP mode, 10-bit in normal and in HR mode.
> - Sampling frequency: same as ODR

### file: temp.c

Enable and read built-in temperature sensor

> - Operating range: -40 to 85°C
> - Step size: ±1°C

### Inertial interrupts


| aoi | en_6d | interrupt mode          |
|-----|-------|-------------------------|
|  0  |   0   | OR combination          |
|  0  |   1   | 6d MOVEMENT recognition |
|  1  |   0   | AND combination         |
|  1  |   1   | 6d POSITION recognition |


#### OR combination

An interrupt is generated when at least one of the configured axes is at or above the threshold level.

#### 6D MOVEMENT recognition

An interrupt is generated when the device moves from a direction (known or unknown) to a different known direction. The interrupt is only active for 1/ODR.

#### AND combination

An interrupt is generated when all of the configures axes are at or above the threshold level.

#### 6D POSITION recognition

An interrupt is generated when the device is "stable" in a known direction. The interrupt is active as long as the direction is maintained.

There are two interrupt registers, `int1` and `int2` that can be configured for inertial interrupts. The config structs are identical and contain the fields: `zh`, `zl`, `yh`, `yl`, `xh`, `xl`, and more. `zh` stands for `Z_axis_high` and `zl` stands for `Z_axis_low`. If both are enabled, the device will generate an interrupt upon Z-axis acceleration exceeding `threshold`, or upon Z-axis acceleration reading at or below `-threshold` (in OR mode. Not possible in AND mode).

### file: inertial-wakeup.c

Inertial interrupt example in OR mode (easily changed to AND mode) with configurable axes, threshold and minimum acceleration duration.

### file: free-fall.c

Inertial interrupt example activating upon free-fall. It works by using an AND mode interrupt of all the negative axes and comparing them to a threshold value (in the case of negative axis the threshold is multiplied by -1), recommended to be at 350mg (for >30 ms) and activating when the experienced negative acceleration is greater (abs. sense) than the negative threshold.