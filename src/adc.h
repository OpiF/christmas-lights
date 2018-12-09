#ifndef _GUARD_ADC_H_
#define _GUARD_ADC_H_

#include <avr/io.h>

void adc_enable();
void adc_disable();
uint16_t adc_read();

#endif
