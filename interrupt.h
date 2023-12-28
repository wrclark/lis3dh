#ifndef INTERRUPT_H
#define INTERRUPT_H

/*
    Note: `pin' is GPIO No.
*/
int int_register(int pin);
int int_unregister(int pin);
int int_poll(int pin);

#endif