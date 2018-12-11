#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "src/adc.h"
#include "src/state.h"

void setup() {
    // cli(); // not required because default SREG value is 0;

    WDTCR = _BV(WDCE);
    WDTCR = _BV(WDP2) | _BV(WDP1) | _BV(WDTIE);

    TCCR0A = _BV(WGM01) | _BV(WGM00) | _BV(COM0B1) | _BV(COM0B0);
    TCCR0B = _BV(CS00);
    DDRB = _BV(DDB1)  | _BV(DDB2);
    DIDR0 = _BV(ADC2D);

    sei();
}

void loop() {
    stateFunction();
}

int main(void) {
    setup();

    for (;;) {
        loop();
        adc_disable();
        sleep_mode();
    }
}
