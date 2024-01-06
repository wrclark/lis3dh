# lis3dh/example

### file: simple.c
Basic example of how to use this device

# FIFO
Instead of polling for every single [x y z] set, a FIFO with programmable capacity ("watermark") can be used, and then dumped into memory once full.

All FIFO readings use 10-bit resolution regardless of the mode set in `cfg.mode`.

The watermark level can be adjusted to a value [1-32] (0 disables FIFO) by modifying the `cfg.fifo.size` property before calling `lis3dh_configure()`.

The FIFO "engine" samples/appends another set of [x y z] values at 1/ODR. The maximum ODR supported by the FIFO "engine" is 200 Hz.

| FIFO mode        |  symbol               | description                |
|------------------|-----------------------|----------------------------|
|  Bypass          |   `LIS3DH_FIFO_MODE_BYPASS` | FIFO is inoperational      |
|  FIFO            |   `LIS3DH_FIFO_MODE_FIFO`   | FIFO can be read/emptied at any time but once overrun has to be reset. See file: `fifo-mode-fifo.c`     |
|  Stream          |   `LIS3DH_FIFO_MODE_STREAM` | FIFO continously writes new data at 1/ODR and will overwrite old data until it is read/emptied. See file: `fifo-mode-stream.c`    |
|  Stream_to_FIFO  |   `LIS3DH_FIFO_STREAM_TO_FIFO`  | FIFO behaves like Stream mode until a set interrupt is activated, then changes to a mode FIFO. |

Note: FIFO will not trigger a watermark interrupt (`pin1.wtm`) if the FIFO size is default (32; maximum size). To use the watermark interrupt, the FIFO size has to be between [1-31]. An overrun interrupt (`pin1.overrun`) will always trigger when the FIFO is full, regardless of programmed capacity.

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

There are two interrupt registers, `int1` and `int2` that can be configured for inertial interrupts. The config structs are identical and contain the fields: `zh`, `zl`, `yh`, `yl`, `xh`, `xl`, and more. `zh` stands for `Z_axis_high` and `zl` stands for `Z_axis_low`. If both are enabled, the device will generate an interrupt upon Z-axis acceleration exceeding `threshold`, or upon Z-axis acceleration reading at or below `-threshold` (in OR mode. Not possible in AND mode).


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

### file: inertial-wakeup.c

Inertial interrupt example in OR mode (easily changed to AND mode) with configurable axes, threshold and minimum acceleration duration.

### file: free-fall.c

Inertial interrupt example activating upon free-fall. It works by using an AND mode interrupt of all the negative axes and comparing them to a threshold value (in the case of negative axis the threshold is multiplied by -1), recommended to be at 350mg (for >30 ms) and activating when the experienced negative acceleration is greater (abs. sense) than the negative threshold.

### file: 6d-movement.c

Inertial interrupt example, generates an interrupt when some acceleration, `threshold` is experienced on any configured axis for `duration` time. Supposedly the device knows what a "known" direction is.

### file: 6d-position.c

Inertial interrupt example, the interrupt line is kept active so long as the device is stable (ie acceleration on configured axes does not exceed `threshold` for `duration` time).

---

### 4D detection

4D detection is a subset of 6D detection meant for detecting portrait/landscape screen rotations on mobile phones, etc. It functionally works the same as the 6D modes, except that detection along the Z-axis is disabled.

### file: 4d-movement.c

Inertial interrupt example, generates an interrupt when some acceleration, `threshold` is experienced on any configured axis for `duration` time. Supposedly the device knows what a "known" direction is.

### file: 4d-position.c

Inertial interrupt example, the interrupt line is kept active so long as the device is stable (ie acceleration on configured axes does not exceed `threshold` for `duration` time).

