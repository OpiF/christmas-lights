#include <util/delay.h>
#include <avr/sleep.h>
#include "src/pwm.h"
#include "src/clock.h"
#include "src/state.h"

static State currentState = State::Init;
static State previousState = State::Init;

static uint32_t lastTransition = 0;
static bool wasResting = false;

//on-off times
const uint32_t workTime = 3_h + 45_min;
const uint32_t breakTime = 30_min;
const uint32_t offTime = 16_h;
const uint32_t recurringBreak = 1_h;
const uint32_t recurringBreakWindow = 10_sec;

void stateTransition(State newState)
{
    switch (newState) {
        case State::Init:
            set_sleep_mode(SLEEP_MODE_IDLE);
            wasResting = false;
            clock_reset();

            break;
        case State::Working:
            set_sleep_mode(SLEEP_MODE_IDLE);
            changeTarget(targetHigh);
            PORTB |= _BV(PB2);

            break;
        case State::Resting:
            set_sleep_mode(SLEEP_MODE_IDLE);
            changeTarget(targetLow);
            PORTB |= _BV(PB2);
            blink(2);

            break;
        case State::Sleep:
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            changeTarget(0);
            PORTB &= ~_BV(PB2);

            break;
    }

    previousState = currentState;
    currentState = newState;
    lastTransition = clock();
}

void stateFunction()
{
    switch (currentState) {
        case State::Init:
            stateTransition(State::Working);

            break;
        case State::Working:
            if (clock() > lastTransition + workTime) {
                if (wasResting) {
                    stateTransition(State::Sleep);
                } else {
                    stateTransition(State::Resting);
                }
            } else if (clock() > recurringBreak && clock() % recurringBreak < recurringBreakWindow) {
                blink(32);
            }
            adjustPWM();

            break;
        case State::Resting:
            if (clock() > lastTransition + breakTime) {
                wasResting = true;
                stateTransition(State::Working);
            }
            adjustPWM();

            break;
        case State::Sleep:
            if (clock() > lastTransition + offTime) {
                stateTransition(State::Init);
            }

            break;
        default:
            stateTransition(State::Init);

            break;
    }
}
