#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define STATE_SETUP	0
#define STATE_HIGH_INTENSITY	1
#define STATE_LOW_INTENSITY 2
#define STATE_HIGH_INTENSITY2	3
#define STATE_WAITING	4


void adc_enable();
void adc_disable();
uint16_t adc_read();
uint16_t adc_avg();
void clearms();
uint32_t getms();
uint8_t guessTargetPWM(uint16_t target);
void changeTarget(uint16_t target);

uint32_t volatile ms = 0;
uint32_t lastMeasurement = 0;
uint32_t currentTimeout = 0;
uint16_t currentTarget = 0;
uint8_t currentPWM = 0;
uint8_t currentState = STATE_HIGH_INTENSITY;
uint8_t previousState = STATE_SETUP;
uint8_t nextState = STATE_SETUP;

const uint8_t maxPWM = 64;
const uint16_t targetLow = 185;
const uint16_t targetHigh = 556;
const uint32_t measurementDelay = 2l * 1000l;

//on-off times
const uint32_t highTime = 3l * 60l * 60l * 1000l;
const uint32_t lowTime = 30l * 60l * 1000l;
const uint32_t offTime = 16l * 60l * 60l * 1000l;
const uint32_t recurringDrop = 1l * 60l * 60l * 1000l;
const uint32_t recurringDropWindow = 10l * 1000l;
// const uint32_t highTime = 2l * 60l * 1000l;
// const uint32_t lowTime = 1l * 60l * 1000l;
// const uint32_t offTime = 2l * 60l * 1000l;

ISR(WDT_vect) {
    ms += 1000l + 185l; //korekta zegara
}

void setup() {
    cli();

    WDTCR = _BV(WDCE);
    WDTCR = _BV(WDP2) | _BV(WDP1) | _BV(WDTIE);

    TCCR0A = _BV(WGM01) | _BV(WGM00) | _BV(COM0B1) | _BV(COM0B0)/*  | _BV(COM0A1) | _BV(COM0A0) */;
    TCCR0B = _BV(CS00);
    DDRB = _BV(DDB1)/*  | _BV(DDB0) */;
    DIDR0 = _BV(ADC2D);

    changeTarget(targetHigh);

    sei();
}

void loop() {
    if (currentState != previousState) {
        switch(currentState) {
            case STATE_HIGH_INTENSITY:
                changeTarget(targetHigh);
                currentTimeout = highTime;
                nextState = STATE_LOW_INTENSITY;

                set_sleep_mode(SLEEP_MODE_IDLE);

                break;
            case STATE_LOW_INTENSITY:
                changeTarget(targetLow);
                currentTimeout = lowTime;
                nextState = STATE_HIGH_INTENSITY2;

                set_sleep_mode(SLEEP_MODE_IDLE);

                break;
            case STATE_HIGH_INTENSITY2:
                changeTarget(targetHigh);
                currentTimeout = highTime;
                nextState = STATE_WAITING;

                set_sleep_mode(SLEEP_MODE_IDLE);

                break;
            case STATE_WAITING:
                changeTarget(0);
                currentTimeout = highTime;
                nextState = STATE_HIGH_INTENSITY;

                set_sleep_mode(SLEEP_MODE_PWR_DOWN);

                break;
            default:
                currentState = STATE_HIGH_INTENSITY;

                break;
        }

        clearms();
        lastMeasurement = 0;
        previousState = currentState;
    }

    uint32_t tms = getms();
    if (tms > currentTimeout) {
        currentState = nextState;

        return;
    } else if (tms > recurringDrop && tms % recurringDrop < recurringDropWindow) {
        uint8_t oldPWM = currentPWM;
        for (int i = 0; i < 16; i++){
            while (currentPWM > 0) {
                currentPWM--;
                OCR0B = 255 - currentPWM;
                _delay_ms(50);
            }
            while (currentPWM < 127) {
                currentPWM++;
                OCR0B = 255 - currentPWM;
                _delay_ms(50);
            }
        }
        currentPWM = oldPWM;
    }

    if (lastMeasurement + measurementDelay <= getms()) {
        lastMeasurement = getms();

        adc_enable();
        uint16_t m = adc_read();
        if (m < currentTarget) {
            currentPWM++;
        } else if (m > currentTarget) {
            currentPWM--;
        }

        currentPWM = (currentPWM < maxPWM)? currentPWM : maxPWM;
        currentPWM = (currentPWM < 0)? 0 : currentPWM;
        OCR0B = 255 - currentPWM;
        // OCR0A = 255 - (m >> 2);
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
#ifdef __AVR_HAVE_PRR_PRADC
    PRR &= ~_BV(PRADC);
#endif
    ADMUX = _BV(REFS0) | /* _BV(ADLAR) | */ _BV(MUX1);
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

uint16_t adc_avg() {
    int32_t sum = 0;
    // const int count = 16;

    // for (int i = 0; i < count; i++) {
    //     sum += adc_read();
    // }

    // sum /= count;

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

uint32_t getms()
{
    uint32_t tmp;

    cli();
    tmp = ms;
    sei();

    return tmp;
}

void clearms()
{
    cli();
    ms = 0;
    sei();
}
