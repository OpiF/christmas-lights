#include <avr/io.h>
#include "src/adc.h"

void adc_enable() {
#ifdef __AVR_HAVE_PRR_PRADC
    PRR &= ~_BV(PRADC);
#endif
    ADMUX = _BV(REFS0) | _BV(MUX1);
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

// void adc_switch(uint8_t pin)
// {
//     switch (pin) {
//         case 0:
//             ADMUX &= ~(_BV(MUX1) | _BV(MUX0));

//             break;
//         case 1:
//             ADMUX &= ~_BV(MUX1);
//             ADMUX |= _BV(MUX0);

//             break;
//         case 2:
//             ADMUX &= ~_BV(MUX0);
//             ADMUX |= _BV(MUX1);

//         case 3:
//             ADMUX |= _BV(MUX1) | _BV(MUX0);

//             break;
//         default:
//             break;
//     }
// }

uint16_t adc_read() {
    ADCSRA |= _BV(ADSC);

    while (ADCSRA & _BV(ADSC));

    return ADC;
}
