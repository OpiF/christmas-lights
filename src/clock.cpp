#include <avr/interrupt.h>
#include "clock.h"

static volatile uint32_t ms = 0;
static const uint16_t ms_per_tick = 1000ul + 185ul; // korekta zegara

ISR(WDT_vect) {
    ms += ms_per_tick;
}

uint32_t clock()
{
    uint32_t tmp;
    uint8_t tSREG = SREG;

    cli();
    tmp = ms;
    SREG = tSREG;

    return tmp;
}

void clock_reset()
{
    uint8_t tSREG = SREG;

    cli();
    ms = 0;
    SREG = tSREG;
}
