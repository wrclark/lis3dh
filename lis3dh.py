# LIS3DH

from time import sleep
from machine import I2C
import struct

LIS3DH_I2C_ADDR = 0x18

sda_pin = machine.Pin(16) 
scl_pin = machine.Pin(17)

i2c = I2C(0, scl=scl_pin, sda=sda_pin, freq=400000)

def read(reg,  n):
    return i2c.readfrom_mem(LIS3DH_I2C_ADDR, reg, n)

def write(reg, val):
    i2c.writeto_mem(LIS3DH_I2C_ADDR, reg, val)

# WHO_AM_I
if read(0x0F, 1)[0] == 0x33:
    print("found LIS3DH")
