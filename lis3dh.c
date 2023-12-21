#include <stdio.h>
#include "lis3dh.h"
#include "registers.h"

int lis3dh_init(lis3dh_t *lis3dh) {
    uint8_t result;
    int err = 0;

    lis3dh->dev.init();

    err |= lis3dh->dev.read(REG_WHO_AM_I, &result, 1);
    if (result != 0x33) {
        err |= 1;
    }

    return err;
}

int lis3dh_deinit(lis3dh_t *lis3dh) {
    lis3dh->dev.deinit();
    return 0;
}