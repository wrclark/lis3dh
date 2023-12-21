#include "lis3dh.h"

int lis3dh_init(lis3dh_t *lis3dh) {
    lis3dh->cfg.rate = 0;
    return 0;
}

int lis3dh_deinit(lis3dh_t *lis3dh) {
    lis3dh->cfg.rate = 0;
    return 0;
}