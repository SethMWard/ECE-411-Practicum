#ifndef _TRAINING_DEVICE_H
#define _TRAINING_DEVICE_H

#define setbit(reg, bit) reg |= (1 << bit)
#define clearbit(reg, bit) reg &= ~(1 << bit)
#define togglebit(reg, bit) reg ^= (1 << bit)
#define F_CPU 1000000UL

void armDevice(void);

#endif