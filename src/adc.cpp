#include <avr/io.h>
#include "src/adc.h"

void adc_enable() {
#ifdef __AVR_HAVE_PRR_PRADC
    PRR &= ~_BV(PRADC);
#endif
    ADMUX = _BV(REFS0);
    ADCSRA = _BV(ADPS1) | _BV(ADPS0) | _BV(ADEN);

    //throw-away reading
    adc_read();
}

void adc_disable() {
    ADCSRA &= ~_BV(ADEN);
#ifdef __AVR_HAVE_PRR_PRADC
    PRR |= _BV(PRADC);
#endif
}

uint16_t adc_read() {
    ADCSRA |= _BV(ADSC);

    while (ADCSRA & _BV(ADSC));

    return ADC;
}
