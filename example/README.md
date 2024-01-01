# lis3dh/example

### simple.c
Basic examples of how to use this device

### fifo.c
Instead of polling for every single [x y z] set, a FIFO with programmable capacity ("watermark") can be used like such: All FIFO readings use 10-bit resolution regardless of the mode set in `lis.cfg.mode`. The watermark level can also be adjusted to a value [0-31] inclusive by modifying the `lis.cfg.fifo.fth` property before calling configure().

The LIS3DH can optionally apply a HP filter on the sample data. It can be used to greatly reduce the "DC acceleration" present.

### interrupts.c
The LIS3DH supports two different interrupt "output pins," `INT1` and `INT2`. The appropriate flag must be set in either `cfg.int1` or `cfg.int2` (only one of such flags can be set at a time!) and the interrupt source must be configured to trigger into `INT1` or `INT2`. Below is example code that listens and receives an interrupt when the FIFO watermark is reached i.e. it is full.

### single-click.c

Set up single-click detection using HP filter