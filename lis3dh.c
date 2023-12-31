#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "lis3dh.h"
#include "registers.h"


int lis3dh_init(lis3dh_t *lis3dh) {
    uint8_t result;
    int err = 0;

    /* if init has been given, check it */
    if (lis3dh->dev.init != NULL) {
        if (lis3dh->dev.init() != 0) {
            return 1;
        }
    }

    /* check WHO_AM_I equal to 0x33 */
    err |= lis3dh->dev.read(REG_WHO_AM_I, &result, 1);
    if (result != 0x33) {
       return 1;
    }

    /* zero device struct */
    memset(&lis3dh->acc, 0, sizeof lis3dh->acc);
    memset(&lis3dh->cfg, 0, sizeof lis3dh->cfg);
    memset(&lis3dh->adc, 0, sizeof lis3dh->adc);

    lis3dh->cfg.fifo.mode = 0xFF; /* in use if neq 0xFF */
    lis3dh->cfg.fifo.fth = 31; /* default watermark level. */

    lis3dh->cfg.filter.mode = 0xFF; /* in use if neq 0xFF */
    lis3dh->cfg.filter.fds = 1; /* bypass OFF by default */

    return err;
}

int lis3dh_configure(lis3dh_t *lis3dh) {
    uint8_t ctrl_reg1, ctrl_reg2, ctrl_reg3;
    uint8_t ctrl_reg4, ctrl_reg5, ctrl_reg6;
    uint8_t ctrl_reg0, temp_cfg_reg;
    uint8_t fifo_ctrl_reg, int1_cfg, int2_cfg;
    uint8_t int1_ths, int2_ths, int1_dur, int2_dur;
    uint8_t click_cfg, click_ths;
    uint8_t time_limit, act_ths;
    int err = 0;

    /* the 0x07 enables Z, Y and X axis in that order */
    ctrl_reg1 = (lis3dh->cfg.rate << 4) | 0x07;
    ctrl_reg2 = 0;
    ctrl_reg3 = 0;
    ctrl_reg4 = (lis3dh->cfg.range << 4);
    ctrl_reg5 = 0;
    ctrl_reg6 = 0;
    fifo_ctrl_reg = 0;
    int1_cfg = 0;
    int2_cfg = 0;
    int1_ths = 0;
    int2_ths = 0;
    int1_dur = 0;
    int2_dur = 0;
    click_cfg = 0;
    click_ths = 0;
    time_limit = 0;
    act_ths = 0;
    ctrl_reg0 = 0;
    temp_cfg_reg = 0;

    /* determine whether to enable ADC or TEMP sensor */
    temp_cfg_reg |= (lis3dh->cfg.en_adc & 1) << 7;
    temp_cfg_reg |= (lis3dh->cfg.en_temp & 1) << 6;

    /* set time limit */
    time_limit |= (lis3dh->cfg.time_limit & 0x7F);

    act_ths |= (lis3dh->cfg.act_ths & 0x7F); 

    /* set click config register */
    click_cfg |= (lis3dh->cfg.click.xs & 1);
    click_cfg |= (lis3dh->cfg.click.xd & 1) << 1;
    click_cfg |= (lis3dh->cfg.click.ys & 1) << 2;
    click_cfg |= (lis3dh->cfg.click.yd & 1) << 3;
    click_cfg |= (lis3dh->cfg.click.zs & 1) << 4;
    click_cfg |= (lis3dh->cfg.click.zd & 1) << 5;

    /* CLICK threshold */
    click_ths |= (lis3dh->cfg.click_ths & 0x7F);
    click_ths |= (lis3dh->cfg.click.latch & 1) << 7;


    /* set interrupt registers */
    ctrl_reg3 |= (lis3dh->cfg.pin1.click & 1) << 7;
    ctrl_reg3 |= (lis3dh->cfg.pin1.ia1 & 1) << 6;
    ctrl_reg3 |= (lis3dh->cfg.pin1.ia2 & 1) << 5;
    ctrl_reg3 |= (lis3dh->cfg.pin1.drdy_zyxda & 1) << 4;
    ctrl_reg3 |= (lis3dh->cfg.pin1.drdy_321 & 1) << 3;
    ctrl_reg3 |= (lis3dh->cfg.pin1.wtm & 1) << 2;
    ctrl_reg3 |= (lis3dh->cfg.pin1.overrun & 1) << 1;

    ctrl_reg6 |= (lis3dh->cfg.pin2.click & 1) << 7;
    ctrl_reg6 |= (lis3dh->cfg.pin2.ia1 & 1) << 6;
    ctrl_reg6 |= (lis3dh->cfg.pin2.ia2 & 1) << 5;
    ctrl_reg6 |= (lis3dh->cfg.pin2.boot & 1) << 4;
    ctrl_reg6 |= (lis3dh->cfg.pin2.act & 1) << 3;
    ctrl_reg6 |= (lis3dh->cfg.pin2.polarity & 1) << 1;

    /* set some of CTRL_REG5 */
    ctrl_reg5 |= (lis3dh->cfg.int2.en_4d & 1);
    ctrl_reg5 |= (lis3dh->cfg.pin2.latch & 1) << 1;
    ctrl_reg5 |= (lis3dh->cfg.int1.en_4d & 1) << 2;
    ctrl_reg5 |= (lis3dh->cfg.pin1.latch & 1) << 3;
    

    /* set INT1_CFG and INT2_CFG */
    int1_cfg |= (lis3dh->cfg.int1.xl & 1);
    int1_cfg |= (lis3dh->cfg.int1.xh & 1) << 1;
    int1_cfg |= (lis3dh->cfg.int1.yl & 1) << 2;
    int1_cfg |= (lis3dh->cfg.int1.yh & 1) << 3;
    int1_cfg |= (lis3dh->cfg.int1.zl & 1) << 4;
    int1_cfg |= (lis3dh->cfg.int1.zh & 1) << 5;
    int1_cfg |= (lis3dh->cfg.int1.en_6d & 1) << 6;
    int1_cfg |= (lis3dh->cfg.int1.aoi & 1) << 7;

    int2_cfg |= (lis3dh->cfg.int2.xl & 1);
    int2_cfg |= (lis3dh->cfg.int2.xh & 1) << 1;
    int2_cfg |= (lis3dh->cfg.int2.yl & 1) << 2;
    int2_cfg |= (lis3dh->cfg.int2.yh & 1) << 3;
    int2_cfg |= (lis3dh->cfg.int2.zl & 1) << 4;
    int2_cfg |= (lis3dh->cfg.int2.zh & 1) << 5;
    int2_cfg |= (lis3dh->cfg.int2.en_6d & 1) << 6;
    int2_cfg |= (lis3dh->cfg.int2.aoi & 1) << 7;

    int1_dur = lis3dh->cfg.int1_dur & 0x7F; 
    int2_dur = lis3dh->cfg.int2_dur & 0x7F;

    int1_ths = lis3dh->cfg.int1_ths & 0x7F; 
    int2_ths = lis3dh->cfg.int2_ths & 0x7F;

    /* set enable FIFO */
    if (lis3dh->cfg.fifo.mode != 0xFF) {
        ctrl_reg5 |= 0x40; /* bit FIFO_EN */
        
        /* restrict maximum fifo size */
        if (lis3dh->cfg.fifo.fth > 31) {
            lis3dh->cfg.fifo.fth = 31;
        }

        fifo_ctrl_reg |= (lis3dh->cfg.fifo.fth);
        fifo_ctrl_reg |= (lis3dh->cfg.fifo.mode << 6);
        fifo_ctrl_reg |= ((lis3dh->cfg.fifo.trig & 1) << 5);
    }

    /* set enable filter */
    if (lis3dh->cfg.filter.mode != 0xFF) {
        ctrl_reg2 |= ((lis3dh->cfg.filter.mode & 0x03) << 6);
        ctrl_reg2 |= ((lis3dh->cfg.filter.cutoff & 0x03) << 4);
        ctrl_reg2 |= ((lis3dh->cfg.filter.fds & 1) << 3);
        ctrl_reg2 |= ((lis3dh->cfg.filter.click & 1) << 2);
        ctrl_reg2 |= ((lis3dh->cfg.filter.ia1 & 1) << 1);
        ctrl_reg2 |= (lis3dh->cfg.filter.ia2 & 1);
    }

    /* always set block update (BDU) */
    ctrl_reg4 |= 0x80;

    /* set high resolution */
    if (lis3dh->cfg.mode == LIS3DH_MODE_HR) {
        ctrl_reg4 |= 0x08;
    }

    /* set LPen */
    if (lis3dh->cfg.mode == LIS3DH_MODE_LP) {
        ctrl_reg1 |= 0x08;
    }

    /* ctrl_reg0 is 0x10 | (SDO << 7) */
    ctrl_reg0 |= 0x10;
    ctrl_reg0 |= (lis3dh->cfg.sdo_pullup & 1) << 7;

    /* write these before the control regs that start the device */
    err |= lis3dh->dev.write(REG_FIFO_CTRL_REG, fifo_ctrl_reg);
    err |= lis3dh->dev.write(REG_INT1_CFG, int1_cfg);
    err |= lis3dh->dev.write(REG_INT1_THS, int1_ths);
    err |= lis3dh->dev.write(REG_INT1_DURATION, int1_dur);
    err |= lis3dh->dev.write(REG_INT2_CFG, int2_cfg);
    err |= lis3dh->dev.write(REG_INT2_THS, int2_ths);
    err |= lis3dh->dev.write(REG_INT2_DURATION, int2_dur);
    err |= lis3dh->dev.write(REG_CLICK_CFG, click_cfg);
    err |= lis3dh->dev.write(REG_CLICK_THS, click_ths);
    err |= lis3dh->dev.write(REG_TIME_LIMIT, time_limit);
    err |= lis3dh->dev.write(REG_TIME_LATENCY, lis3dh->cfg.time_latency);
    err |= lis3dh->dev.write(REG_TIME_WINDOW, lis3dh->cfg.time_window);
    err |= lis3dh->dev.write(REG_ACT_THS, act_ths);
    err |= lis3dh->dev.write(REG_ACT_DUR, lis3dh->cfg.act_dur);
    err |= lis3dh->dev.write(REG_TEMP_CFG_REG, temp_cfg_reg);

    err |= lis3dh->dev.write(REG_CTRL_REG0, ctrl_reg0);
    err |= lis3dh->dev.write(REG_CTRL_REG1, ctrl_reg1);
    err |= lis3dh->dev.write(REG_CTRL_REG2, ctrl_reg2);
    err |= lis3dh->dev.write(REG_CTRL_REG3, ctrl_reg3);
    err |= lis3dh->dev.write(REG_CTRL_REG4, ctrl_reg4);
    err |= lis3dh->dev.write(REG_CTRL_REG5, ctrl_reg5);
    err |= lis3dh->dev.write(REG_CTRL_REG6, ctrl_reg6);

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
static uint8_t acc_shift(uint8_t mode) {
    switch (mode) {
        case LIS3DH_MODE_HR:     return 4;  /* i12 */
        case LIS3DH_MODE_NORMAL: return 6;  /* i10 */
        case LIS3DH_MODE_LP:     return 8;  /* i8 */
        default:                 return 0;
    }
}

/* returns a scalar that when multiplied with axis reading
   turns it to a multiple of mg. */
static uint8_t acc_sensitivity(uint8_t mode, uint8_t range) {
    switch (range) {
        case LIS3DH_FS_2G:    return (mode == LIS3DH_MODE_LP) ? 16  : (mode == LIS3DH_MODE_NORMAL) ? 4  : 1;
        case LIS3DH_FS_4G:    return (mode == LIS3DH_MODE_LP) ? 32  : (mode == LIS3DH_MODE_NORMAL) ? 8  : 2;
        case LIS3DH_FS_8G:    return (mode == LIS3DH_MODE_LP) ? 64  : (mode == LIS3DH_MODE_NORMAL) ? 16 : 4;
        case LIS3DH_FS_16G:   return (mode == LIS3DH_MODE_LP) ? 192 : (mode == LIS3DH_MODE_NORMAL) ? 48 : 12;
        default:              return 0;
    }
}


int lis3dh_read(lis3dh_t *lis3dh) {
    uint8_t data[6];
    int32_t x, y, z;
    uint8_t scale, sens;
    int err = 0;

    scale = acc_shift(lis3dh->cfg.mode);
    sens = acc_sensitivity(lis3dh->cfg.mode, lis3dh->cfg.range);

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
    int32_t x, y, z;
    uint8_t scale, sens;
    uint8_t data[192]; /* max size */
    int err = 0;
    int i, idx;

    /* FIFO is always 10-bit */
    scale = 6;
    sens = acc_sensitivity(lis3dh->cfg.mode, lis3dh->cfg.range);

    /* fifo buffer is max 31 */
    fifo->size = lis3dh->cfg.fifo.fth > 31 ? 31 : lis3dh->cfg.fifo.fth;

    /* must set MSbit of the address to multi-read and 
       have the device auto-increment the address.
       see 5.1.5 in datasheet. */
    err |= lis3dh->dev.read(REG_OUT_X_L | 0x80, data, 192);

    for (i=0, idx=0; i<fifo->size * 6; i+=6, idx++) {
        x = (((int16_t)((data[i + 0] << 8) | data[i + 1])) >> scale) * sens;
        y = (((int16_t)((data[i + 2] << 8) | data[i + 3])) >> scale) * sens;
        z = (((int16_t)((data[i + 4] << 8) | data[i + 5])) >> scale) * sens;

        fifo->x[idx] = ((float)x) / 1000.0;
        fifo->y[idx] = ((float)y) / 1000.0;
        fifo->z[idx] = ((float)z) / 1000.0;
    }

    return err;
}

/* if NULL, this function doesn't have to be called */
int lis3dh_deinit(lis3dh_t *lis3dh) {
    if (lis3dh->dev.deinit != NULL) {
        return lis3dh->dev.deinit();
    }

    return 0;
}

/* read INT1_SRC to clear latched INT1 irq */
int lis3dh_clear_int1(lis3dh_t *lis3dh) {
    uint8_t res;
    return lis3dh->dev.read(REG_INT1_SRC, &res, 1);
}

/* read INT2_SRC to clear latched INT2 irq */
int lis3dh_clear_int2(lis3dh_t *lis3dh) {
    uint8_t res;
    return lis3dh->dev.read(REG_INT2_SRC, &res, 1);
}

/* read REFERENCE reg to reset HP filter in REFERENCE mode
   it uses the --current-- acceleration as the base in the filter */
int lis3dh_reference(lis3dh_t *lis3dh) {
    uint8_t res;
    return lis3dh->dev.read(REG_REFERENCE, &res, 1);
}

/* reset user regs and reload trim params */
int lis3dh_reset(lis3dh_t *lis3dh) {
    int err = 0;

    /* set BOOT bit so device reloads internal trim parameters */
    err |= lis3dh->dev.write(REG_CTRL_REG5, 0x80);

    /* write default values to rw regs */
    err |= lis3dh->dev.write(REG_CTRL_REG0, 0x10);
    err |= lis3dh->dev.write(REG_CTRL_REG1, 0x07);
    err |= lis3dh->dev.write(REG_CTRL_REG2, 0x00);
    err |= lis3dh->dev.write(REG_CTRL_REG3, 0x00);
    err |= lis3dh->dev.write(REG_CTRL_REG4, 0x00);
    err |= lis3dh->dev.write(REG_CTRL_REG5, 0x00);
    err |= lis3dh->dev.write(REG_CTRL_REG6, 0x00);
    err |= lis3dh->dev.write(REG_FIFO_CTRL_REG, 0x00);
    err |= lis3dh->dev.write(REG_INT1_CFG, 0x00);
    err |= lis3dh->dev.write(REG_INT1_THS, 0x00);
    err |= lis3dh->dev.write(REG_INT1_DURATION, 0x00);
    err |= lis3dh->dev.write(REG_INT2_CFG, 0x00);
    err |= lis3dh->dev.write(REG_INT2_THS, 0x00);
    err |= lis3dh->dev.write(REG_INT2_DURATION, 0x00);
    err |= lis3dh->dev.write(REG_CLICK_CFG, 0x00);
    err |= lis3dh->dev.write(REG_CLICK_THS, 0x00);
    err |= lis3dh->dev.write(REG_TIME_LIMIT, 0x00);
    err |= lis3dh->dev.write(REG_TIME_LATENCY, 0x00);
    err |= lis3dh->dev.write(REG_TIME_WINDOW, 0x00);
    err |= lis3dh->dev.write(REG_ACT_THS, 0x00);
    err |= lis3dh->dev.write(REG_ACT_DUR, 0x00);
    
    return err;
}

/* read all 3 ADCs and convert readings depending on power mode
   st 1 LSb is equal to 1 millivolt */
int lis3dh_read_adc(lis3dh_t *lis3dh) {
    uint8_t data[6];
    int err = 0;
    uint8_t shift;
    float divisor;
    /* read adc{1,2,3} LSB and MSB */
    err |= lis3dh->dev.read(REG_OUT_ADC1_L | 0x80, data, 6);

    if (lis3dh->cfg.mode == LIS3DH_MODE_LP) {
        shift = 8;
        divisor = 256.0;
    } else {
        shift = 6;
        divisor = 1024.0;
    }

    lis3dh->adc.adc1 = 1200.0 + (400.0 * (float)(((int16_t)(data[1] | (data[0] << 8))) >> shift) / divisor); 
    lis3dh->adc.adc2 = 1200.0 + (400.0 * (float)(((int16_t)(data[3] | (data[2] << 8))) >> shift) / divisor); 
    lis3dh->adc.adc3 = 1200.0 + (400.0 * (float)(((int16_t)(data[5] | (data[4] << 8))) >> shift) / divisor); 

    return err;
}

/* the temperature sensor only reports the difference between its current temp,
   and the factory calibrated temperature, 25 celsius.
   in increments of plus or negative 1 unit celsius.
   the reported temperature is stored inplace of adc3 
   temp sensing is always in 8-bit mode 
   operating range: -40 to 85 celsius */
int lis3dh_read_temp(lis3dh_t *lis3dh) {
    uint8_t data;
    int err = 0;

    err |= lis3dh->dev.read(REG_OUT_ADC3_H, &data, 1);
    lis3dh->adc.adc3 = (float)((int8_t)data) + 25.0;
    return err;
}
