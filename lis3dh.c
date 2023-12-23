#include "lis3dh.h"
#include "registers.h"

/* reset user regs and reload trim params */
static int lis3dh_reset(lis3dh_t *lis3dh) {
    int err = 0;

    /* set BOOT bit so device reloads internal trim parameters */
    err |= lis3dh->dev.write(REG_CTRL_REG5, 0x80);

    /* write default values to rw regs */
    err |= lis3dh->dev.write(REG_CTRL_REG0, 0b10000);
    err |= lis3dh->dev.write(REG_CTRL_REG1, 0b111);
    err |= lis3dh->dev.write(REG_CTRL_REG2, 0);
    err |= lis3dh->dev.write(REG_CTRL_REG3, 0);
    err |= lis3dh->dev.write(REG_CTRL_REG4, 0);
    err |= lis3dh->dev.write(REG_CTRL_REG5, 0);
    err |= lis3dh->dev.write(REG_CTRL_REG6, 0);
    err |= lis3dh->dev.write(REG_FIFO_CTRL_REG, 0);
    err |= lis3dh->dev.write(REG_INT1_CFG, 0);
    err |= lis3dh->dev.write(REG_INT1_THS, 0);
    err |= lis3dh->dev.write(REG_INT1_DURATION, 0);
    err |= lis3dh->dev.write(REG_INT2_CFG, 0);
    err |= lis3dh->dev.write(REG_INT2_THS, 0);
    err |= lis3dh->dev.write(REG_INT2_DURATION, 0);
    err |= lis3dh->dev.write(REG_CLICK_CFG, 0);
    err |= lis3dh->dev.write(REG_CLICK_THS, 0);
    err |= lis3dh->dev.write(REG_TIME_LIMIT, 0);
    err |= lis3dh->dev.write(REG_TIME_LATENCY, 0);
    err |= lis3dh->dev.write(REG_TIME_WINDOW, 0);
    err |= lis3dh->dev.write(REG_ACT_THS, 0);
    err |= lis3dh->dev.write(REG_ACT_DUR, 0);
    
    return err;
}

int lis3dh_init(lis3dh_t *lis3dh) {
    uint8_t result;
    int err = 0;

    if (lis3dh->dev.init() != 0) {
        return 1;
    }

    err |= lis3dh->dev.read(REG_WHO_AM_I, &result, 1);
    if (result != 0x33) {
       return 1;
    }

    /* zero struct */
    lis3dh->acc.x = 0.0;
    lis3dh->acc.y = 0.0;
    lis3dh->acc.z = 0.0;
    lis3dh->cfg.rate = 0;
    lis3dh->cfg.range = 0;
    lis3dh->cfg.mode = 0;
    lis3dh->cfg.fifo.mode = 0xFF; /* in use if neq 0xFF */
    lis3dh->cfg.fifo.trig = 0;
    lis3dh->cfg.fifo.fth = 32; /* default watermark level. */
    lis3dh->cfg.filter.mode = 0xFF; /* in use if neq 0xFF */
    lis3dh->cfg.filter.cutoff = 0;
    lis3dh->cfg.filter.fds = 0;
    lis3dh->cfg.filter.hpclick = 0;
    lis3dh->cfg.filter.ia1 = 0;
    lis3dh->cfg.filter.ia2 = 0;

    err |= lis3dh_reset(lis3dh);

    return err;
}

int lis3dh_configure(lis3dh_t *lis3dh) {

    uint8_t ctrl_reg1, ctrl_reg2, ctrl_reg4, ctrl_reg5;
    uint8_t fifo_ctrl_reg;
    uint8_t ref;
    int err = 0;

    /* last 0b111 enables Z, Y and X axis */
    ctrl_reg1 = 0 | (lis3dh->cfg.rate << 4) | 0b111;
    ctrl_reg2 = 0;
    ctrl_reg4 = 0 | (lis3dh->cfg.range << 4);
    ctrl_reg5 = 0;
    fifo_ctrl_reg = 0;

    /* set enable FIFO */
    if (lis3dh->cfg.fifo.mode != 0xFF) {
        ctrl_reg5 |= 0x40;
        fifo_ctrl_reg |= (lis3dh->cfg.fifo.fth & 0x1F);
        fifo_ctrl_reg |= (lis3dh->cfg.fifo.mode << 6);
        fifo_ctrl_reg |= ((lis3dh->cfg.fifo.trig & 1) << 5);
    }

    /* set enable filter */
    if (lis3dh->cfg.filter.mode != 0xFF) {
        ctrl_reg2 |= ((lis3dh->cfg.filter.mode & 0b11) << 6);
        ctrl_reg2 |= ((lis3dh->cfg.filter.cutoff & 0b11) << 4);
        ctrl_reg2 |= ((lis3dh->cfg.filter.fds & 1) << 3);
        ctrl_reg2 |= ((lis3dh->cfg.filter.hpclick & 1) << 2);
        ctrl_reg2 |= ((lis3dh->cfg.filter.ia1 & 1) << 1);
        ctrl_reg2 |= (lis3dh->cfg.filter.ia2 & 1);
    }

    /* always set block update */
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
    err |= lis3dh->dev.write(REG_CTRL_REG2, ctrl_reg2);
    err |= lis3dh->dev.write(REG_CTRL_REG4, ctrl_reg4);
    err |= lis3dh->dev.write(REG_CTRL_REG5, ctrl_reg5);
    err |= lis3dh->dev.write(REG_FIFO_CTRL_REG, fifo_ctrl_reg);

    /* read REFERENCE to clear internal filter struct */
    err |= lis3dh->dev.read(REG_REFERENCE, &ref, 1);

    /* sleep for a period TBD ~ ODR */
    lis3dh->dev.sleep(50000); /* 50 ms */
    return err;
}

int lis3dh_poll(lis3dh_t *lis3dh) {

    uint8_t status;
    int err = 0;

    do {
        lis3dh->dev.sleep(1000); /* 1 ms */
        err |= lis3dh->dev.read(REG_STATUS_REG, &status, 1);
    } while (!err && !((status >> 3) & 1));

    return err;
}

int lis3dh_poll_fifo(lis3dh_t *lis3dh) {
    uint8_t src;
    int err = 0;

    do {
        lis3dh->dev.sleep(1000); /* 1 ms */
        err |= lis3dh->dev.read(REG_FIFO_SRC_REG, &src, 1);
    } while (!err && !(src >> 6));

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

    lis3dh->acc.x = ((float)x) / 1000.0;
    lis3dh->acc.y = ((float)y) / 1000.0;
    lis3dh->acc.z = ((float)z) / 1000.0;

    return err;
}

int lis3dh_read_fifo(lis3dh_t *lis3dh, struct lis3dh_fifo_data *fifo) {

    int16_t x, y, z;
    uint8_t scale, sens;
    uint8_t data[192]; /* max size */
    int err = 0;
    int i, idx;

    scale = acc_shift(lis3dh);
    sens = acc_sensitivity(lis3dh);

    fifo->size = lis3dh->cfg.fifo.fth;

    /* must set MSbit of the address to multi-read and 
       have the device auto-increment the address.
       see 5.1.5 in datasheet. */
    err |= lis3dh->dev.read(REG_OUT_X_L | 0x80, data, 192);

    for(i=0, idx=0; i<lis3dh->cfg.fifo.fth * 6; i+=6, idx++) {
        x = (((int16_t)((data[i + 0] << 8) | data[i + 1])) >> scale) * sens;
        y = (((int16_t)((data[i + 2] << 8) | data[i + 3])) >> scale) * sens;
        z = (((int16_t)((data[i + 4] << 8) | data[i + 5])) >> scale) * sens;

        fifo->x[idx] = ((float)x) / 1000.0;
        fifo->y[idx] = ((float)y) / 1000.0;
        fifo->z[idx] = ((float)z) / 1000.0;
    }

    return err;
}

int lis3dh_deinit(lis3dh_t *lis3dh) {
    return lis3dh->dev.deinit();
}