#include <avr/io.h>
#include <util/delay.h>
#include "src/adc.h"
#include "src/pwm.h"

static uint16_t currentTarget = 0;
static uint8_t currentPWM = 0;
static const uint8_t maxPWM = 64;
static const uint8_t blinkPWM = 127;

const uint16_t targetLow = 20_mA;
const uint16_t targetHigh = 60_mA;


static uint8_t guessTargetPWM(uint16_t target)
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

void adjustPWM()
{
    adc_enable();

    adc_select<ADCPin::ADC2>();
    uint16_t m = adc_read();
    if (m < currentTarget) {
        currentPWM++;
    } else if (m > currentTarget) {
        currentPWM--;
    }

    currentPWM = (currentPWM < maxPWM)? currentPWM : maxPWM;
    currentPWM = (currentPWM < 0)? 0 : currentPWM;
    OCR0B = 255 - currentPWM;
}

void blink(uint8_t count)
{
    const int delay = 32;
    const uint8_t oldPWM = currentPWM;

    for (uint8_t i = 0; i < count; i++) {
        while (currentPWM > 0) {
            currentPWM--;
            OCR0B = 255 - currentPWM;
            _delay_ms(delay);
        }
        while (currentPWM < blinkPWM) {
            currentPWM++;
            OCR0B = 255 - currentPWM;
            _delay_ms(delay);
        }
    }

    // smooth ease-out
    while (currentPWM > oldPWM) {
        currentPWM--;
        _delay_ms(delay);
    }
    currentPWM = oldPWM; // just to be sure
}
