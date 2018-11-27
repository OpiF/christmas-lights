#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define STATE_SETUP	0
#define STATE_HIGH_INTENSITY	1
#define STATE_LOW_INTENSITY	2
#define STATE_WAITING	3


void adc_enable();
void adc_disable();
int16_t adc_read();
int16_t adc_avg();
uint8_t guessTargetPWM(uint16_t target);
void changeTarget(uint16_t target);

uint32_t volatile ms = 0;
uint32_t lastMeasurement = 0;
uint16_t currentTarget = 0;
uint8_t currentPWM = 0;
uint8_t currentState = STATE_SETUP;

const uint8_t maxPWM = 64;
const uint16_t targetLow = 185;
const uint16_t targetHigh = 465;
const uint32_t measurementDelay = 5l * 1000l;

//on-off times
const uint32_t highTime = 3l * 60l * 1000l;
const uint32_t lowTime = 1l * 60l * 1000l;
const uint32_t offTime = 2l * 60l * 1000l;

ISR(WDT_vect) {
    ms += 1000l + 300l;
}

void setup() {
    cli();

    WDTCR = _BV(WDCE);
    WDTCR = _BV(WDP2) | _BV(WDP1) | _BV(WDTIE);

    TCCR0A = _BV(WGM01) | _BV(WGM00) | _BV(COM0B1) | _BV(COM0B0);
    TCCR0B = _BV(CS00);
    DDRB = _BV(PB1);

    changeTarget(targetHigh);

    sei();
}

void loop() {
    switch(currentState) {
        case STATE_HIGH_INTENSITY:
            if (ms > highTime) {
                ms = 0;
                currentState = STATE_LOW_INTENSITY;
                changeTarget(targetLow);

                set_sleep_mode(SLEEP_MODE_IDLE);
            }

            break;
        case STATE_LOW_INTENSITY:
            if (ms > lowTime) {
                ms = 0;
                currentState = STATE_WAITING;
                changeTarget(0);

                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            }

            break;
        case STATE_WAITING:
            if (ms <= offTime) {
                return;
            }
	default:
            ms = 0;
            currentState = STATE_HIGH_INTENSITY;
            changeTarget(targetHigh);

            set_sleep_mode(SLEEP_MODE_IDLE);

            break;
    }

    if (lastMeasurement + measurementDelay <= ms) {
        lastMeasurement = ms;

        adc_enable();
        int16_t m = adc_avg();
        if (m < currentTarget) {
            currentPWM++;
        } else if (m > currentTarget) {
            currentPWM--;
        }

        currentPWM = (currentPWM < maxPWM)? currentPWM : maxPWM;
        currentPWM = (currentPWM < 0)? 0 : currentPWM;
        OCR0B = 255 - currentPWM;
    }
}

int main(void) {
    setup();

    for (;;) {
        loop();
        adc_disable();
        sleep_mode();
    }
}

void adc_enable() {
    ADMUX = _BV(REFS0) /* | _BV(ADLAR) */ | _BV(MUX1);
    ADCSRA = _BV(ADPS1) | _BV(ADPS0) | _BV(ADEN);

    //throw-away reading
    adc_read();
}

void adc_disable() {
    ADCSRA &= ~_BV(ADEN);
}

int16_t adc_read() {
    ADCSRA |= _BV(ADSC);

    while (ADCSRA & _BV(ADSC));

    return ADC;
}

int16_t adc_avg() {
    int32_t sum = 0;
    const int count = 20;

    for (int i = 0; i < count; i++) {
        sum += adc_read();
    }

    sum /= count;

    return sum;
}

uint8_t guessTargetPWM(uint16_t target)
{
    int32_t temp = target;
    temp *= maxPWM;
    temp /= 1024;

    return temp & 255;
}

void changeTarget(uint16_t target)
{
    currentTarget = target;
    currentPWM = guessTargetPWM(target);

    currentPWM = (currentPWM < maxPWM)? currentPWM : maxPWM;
    currentPWM = (currentPWM < 0)? 0 : currentPWM;
    OCR0B = 255 - currentPWM;
}
