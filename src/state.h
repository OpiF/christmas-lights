#ifndef _GUARD_STATE_H_
#define _GUARD_STATE_H_

enum class State: uint8_t {
    Init,
    Working,
    Resting,
    Sleep
};

void stateFunction();

#endif
