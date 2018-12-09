#ifndef _GUARD_CLOCK_H_
#define _GUARD_CLOCK_H_

#include <avr/io.h>

void clock_reset();
uint32_t clock();

constexpr uint32_t operator "" _sec(unsigned long long s)
{
    return s * 1000ul;
}

constexpr uint32_t operator "" _min(unsigned long long m)
{
    return m * 60_sec;
}

constexpr uint32_t operator "" _h(unsigned long long h)
{
    return h * 60_min;
}

#endif
