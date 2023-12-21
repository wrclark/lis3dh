#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include "lis3dh.h"
#include "i2c.h"

int main() {

    lis3dh_t lis;

    lis.dev.init = i2c_init;
    lis.dev.read = i2c_read;
    lis.dev.write = i2c_write;
    lis.dev.sleep = usleep;
    lis.dev.deinit = i2c_deinit;


    if (!lis3dh_init(&lis)) {
        puts("OK");
    }

    if (!lis3dh_deinit(&lis)) {
        puts("OK");
    }

    return 0;
}