#include <stdio.h>
#include "lis3dh.h"

int main() {

    lis3dh_t lis;

    if (!lis3dh_init(&lis)) {
        puts("OK");
    }

    if (!lis3dh_deinit(&lis)) {
        puts("OK");
    }

    return 0;
}