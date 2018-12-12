#ifndef _GUARD_PWM_H_
#define _GUARD_PWM_H_

#include "avr/io.h"

void changeTarget(uint16_t target);
void adjustPWM();
void blink(uint8_t count);

extern const uint16_t targetHigh;
extern const uint16_t targetLow;

const uint32_t resistorValue = 4700ul; // mOhm
const uint32_t referenceVoltage = 1100ul; // mV

constexpr uint16_t operator "" _mA(unsigned long long mA)
{
    return (mA * resistorValue * 1024ul) / referenceVoltage / 1000ul;
}

#endif
