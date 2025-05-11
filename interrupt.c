/* 
  SYSFS gpio interrupt example for Raspberry Pi
*/
#define _GNU_SOURCE
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include "interrupt.h"


static int write_file(const char *path, const char *str) {
    int fd = 0;

    if ((fd = open(path, O_WRONLY)) < 0) {
        fprintf(stderr, "unable to open %s\n", path);
        return 1;
    }

    if (write(fd, str, strlen(str)) < 1) {
        fprintf(stderr, "unable to write to %s\n", path);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}

/* Register a sysfs GPIO pin to be interruptable
   similar to:
   $ echo 12 > /sys/class/gpio/export
   $ echo both > /sys/class/gpio12/edge
   $ echo in > /sys/class/gpio12/direction

*/
int int_register(int pin) {

    char path[256];
    char buffer[64];

    /* begin by unregistering supplied pin, just in case */
    (void) int_unregister(pin);

    memset(buffer, 0, 64);
    sprintf(buffer, "%d", pin);
    if (write_file("/sys/class/gpio/export", buffer) != 0) {
        return 1;
    }

    /* sleep 500 ms to let linux set up the dir .. */
    usleep(500000);

    memset(path, 0, 256);
    sprintf(path, "%s%d/%s", "/sys/class/gpio/gpio", pin, "edge");
    if (write_file(path, "both") != 0) {
        return 1;
    }

    memset(path, 0, 256);
    sprintf(path, "%s%d/%s", "/sys/class/gpio/gpio", pin, "direction");
    if (write_file(path, "in") != 0) {
        return 1;
    }


    return 0;
}

/* unregister an interrupt gpio config 
   similar to:
   $ echo 12 > /sys/class/gpio/unexport
*/
int int_unregister(int pin) {
    char buffer[64];

    memset(buffer, 0, 64);
    sprintf(buffer, "%d", pin);
    if (write_file("/sys/class/gpio/unexport", buffer) != 0) {
        return 1;
    }

    usleep(50000); /* sleep 50 ms */

    return 0;
}

/* poll a gpio pin for interrupt event 
   blocking
*/
int int_poll(int pin) {
    int fd;
    struct pollfd pfd;
    char path[256];
    char buf[16];

    memset(path, 0, 256);
    sprintf(path, "%s%d/%s", "/sys/class/gpio/gpio", pin, "value");

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(stderr, "unable to open %s\n", path);
        return 1;
    }

    pfd.fd = fd;
    pfd.events = POLLPRI | POLLERR;

    lseek(fd, 0, SEEK_SET);
    read(fd, buf, sizeof buf);

    poll(&pfd, 1, -1);

    lseek(fd, 0, SEEK_SET);
    read(fd, buf, sizeof buf);

    return 0;
}
