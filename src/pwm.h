#ifndef _GUARD_PWM_H_
#define _GUARD_PWM_H_

#include "avr/io.h"

void changeTarget(uint16_t target);
void adjustPWM();
void blink();

extern const uint16_t targetHigh;
extern const uint16_t targetLow;

#endif
