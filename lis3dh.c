#include <stdio.h>
#include "lis3dh.h"
#include "registers.h"

static int lis3dh_reboot(lis3dh_t *lis3dh) {
    int err = 0;

    err |= lis3dh->dev.write(REG_CTRL_REG5, 0x80);
    lis3dh->dev.sleep(5000); /* sleep 5 ms */
    
    return err;
}

int lis3dh_init(lis3dh_t *lis3dh) {
    uint8_t result;
    int err = 0;

    if (lis3dh->dev.init() != 0) {
        err |= 1;
    }

    err |= lis3dh->dev.read(REG_WHO_AM_I, &result, 1);
    if (result != 0x33) {
        err |= 1;
    }

    err |= lis3dh_reboot(lis3dh);

    return err;
}

int lis3dh_configure(lis3dh_t *lis3dh) {

    uint8_t ctrl_reg1, ctrl_reg4;
    uint8_t ref;
    int err = 0;

    /* last 0b111 enables Z, Y and X axis */
    ctrl_reg1 = 0 | (lis3dh->cfg.rate << 4) | 0b111;
    ctrl_reg4 = 0 | (lis3dh->cfg.range << 4);

    /* set block update */
    ctrl_reg4 |= 0x80;

    /* set high resolution */
    if (lis3dh->cfg.mode == LIS3DH_MODE_HR) {
        ctrl_reg4 |= 0x08;
    }

    /* set LPen */
    if (lis3dh->cfg.mode == LIS3DH_MODE_LP) {
        ctrl_reg1 |= 0b1000;
    }

    err |= lis3dh->dev.write(REG_CTRL_REG1, ctrl_reg1);
    err |= lis3dh->dev.write(REG_CTRL_REG4, ctrl_reg4);

    /* read REFERENCE to clear internal filter struct */
    err |= lis3dh->dev.read(REG_REFERENCE, &ref, 1);

    /* sleep for a period TBD ~ ODR */
    lis3dh->dev.sleep(5000); /* 5 ms */
    return err;
}

int lis3dh_poll(lis3dh_t *lis3dh) {

    uint8_t status;
    int err = 0;

    do {
        err |= lis3dh->dev.read(REG_STATUS_REG, &status, 1);
    } while (!err && !((status >> 3) & 1));

    return err;
}

/* the real size of the int you get back from reading the acc u16 
   depends on the power mode. 
   shift down the 16 bit word by this amount: */
static uint8_t acc_shift(lis3dh_t *lis3dh) {
    switch (lis3dh->cfg.mode) {
        case LIS3DH_MODE_HR:
            return 4; /* i12 */
        case LIS3DH_MODE_NORMAL:
            return 6; /* i10 */
        case LIS3DH_MODE_LP:
            return 8; /* i8 */
    }

    return 0;
}

/* returns a scalar that when multiplied with axis reading
   turns it to a multiple of mg. */
static uint8_t acc_sensitivity(lis3dh_t *lis3dh) {

    uint8_t mode = lis3dh->cfg.mode;

    switch (lis3dh->cfg.range) {
        case LIS3DH_FS_2G:
            if (mode == LIS3DH_MODE_LP) {
                return 16;
            } else if (mode == LIS3DH_MODE_NORMAL) {
                return 4;
            } else {
                return 1;
            }
            break;

        case LIS3DH_FS_4G:
            if (mode == LIS3DH_MODE_LP) {
                return 32;
            } else if (mode == LIS3DH_MODE_NORMAL) {
                return 8;
            } else {
                return 2;
            }
            break;

        case LIS3DH_FS_8G:
            if (mode == LIS3DH_MODE_LP) {
                return 64;
            } else if (mode == LIS3DH_MODE_NORMAL) {
                return 16;
            } else {
                return 4;
            }
            break;

        case LIS3DH_FS_16G:
            if (mode == LIS3DH_MODE_LP) {
                return 192;
            } else if (mode == LIS3DH_MODE_NORMAL) {
                return 48;
            } else {
                return 12;
            }
            break;
    }

    return 0;
}

int lis3dh_read(lis3dh_t *lis3dh) {

    uint8_t data[6];
    int16_t x, y, z;
    uint8_t scale, sens;
    int err = 0;

    scale = acc_shift(lis3dh);
    sens = acc_sensitivity(lis3dh);

    /* must set MSbit of the address to multi-read and 
       have the device auto-increment the address. */
    err |= lis3dh->dev.read(REG_OUT_X_L | 0x80, data, 6);

    /* x,y,z are now in mg */
    x = (((int16_t)((data[0] << 8) | data[1])) >> scale) * sens;
    y = (((int16_t)((data[2] << 8) | data[3])) >> scale) * sens;
    z = (((int16_t)((data[4] << 8) | data[5])) >> scale) * sens;

    lis3dh->acc.x = (double)x / 1000.0;
    lis3dh->acc.y = (double)y / 1000.0;
    lis3dh->acc.z = (double)z / 1000.0;

    return err;
}

int lis3dh_deinit(lis3dh_t *lis3dh) {
    return lis3dh->dev.deinit();
}