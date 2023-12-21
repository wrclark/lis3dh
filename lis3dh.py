# LIS3DH

from time import sleep
from machine import I2C
import struct

LIS3DH_I2C_ADDR = 0x18

# ODR data rate
ODR_POWER_OFF = 0b0000
# all operating modes
ODR_1_HZ = 0b0001
ODR_10_HZ = 0b0010
ODR_25_HZ = 0b0011
ODR_50_HZ = 0b0100
ODR_100_HZ = 0b0101
ODR_200_HZ = 0b0110
ODR_400_HZ = 0b0111
# specific operating modes only
ODR_LP_1600_HZ = 0b1000
ODR_NORM_1344_HZ = 0b1001
ODR_LP_5376_HZ = 0b1001

# acc. scale
FS_2G = 0b00
FS_4G = 0b01
FS_8G = 0b10
FS_16G = 0b11


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


# reboot
write(0x24, struct.pack("B", 0x80))
sleep(0.1)


Z_en = 1
Y_en = 1
X_en = 1
ctrl_reg1 = 0 | (ODR_10_HZ << 4) | (Z_en << 2) | (Y_en << 1) | X_en
write(0x20, struct.pack("B", ctrl_reg1))

ctrl_reg4 = 0 | (FS_16G << 4)
ctrl_reg4 |= 0x80 # set block update
ctrl_reg4 |= 0x08 # set HR mode
write(0x23, struct.pack("B", ctrl_reg4))


sleep(0.2) # 200 ms


while True:
    
    while ((read(0x27, 1)[0] >> 3) & 1) == 0:
        sleep(0.02)
    
    data = read(0x28 | 0x80, 6)
    accX, accY, accZ = struct.unpack("<hhh", data)
    accX >>= 2
    accY >>= 2
    accZ >>= 2
    
    # divide depending on FS to get units of mg
    # this is only for HR mode (maybe)
    # see table 4 in datasheet
    
    # scale output to g
    #scale = 1/(4 * 1000) # FS_2G HR
    #scale = 1/(2 * 1000) # FS_4G HR
    #scale = 1/(1 * 1000) # FS_8G HR
    scale = 1/(0.3333333 * 1000) # FS_16G HR
    accX *= scale
    accY *= scale
    accZ *= scale
    

    print(f"x: {accX:.3f}, y: {accY:.3f}, z: {accZ:.3f}")

    
    sleep(0.05)
