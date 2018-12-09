#ifndef _GUARD_ADC_H_
#define _GUARD_ADC_H_

#include <avr/io.h>

void adc_enable();
void adc_disable();
uint16_t adc_read();

enum class ADCPin {
    ADC0,
    ADC1,
    ADC2,
    ADC3
};

template<ADCPin pin> void adc_select()
{
    switch (pin) {
        case ADCPin::ADC0:
            ADMUX &= ~_BV(MUX1);
            ADMUX &= ~_BV(MUX0);

            break;
        case ADCPin::ADC1:
            ADMUX &= ~_BV(MUX1);
            ADMUX |= _BV(MUX0);

            break;
        case ADCPin::ADC2:
            ADMUX |= _BV(MUX1);
            ADMUX &= ~_BV(MUX0);

            break;
        case ADCPin::ADC3:
            ADMUX |= _BV(MUX1);
            ADMUX |= _BV(MUX0);

            break;
        default:
            break;
    }
}

#endif
