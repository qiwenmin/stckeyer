#include <tnyfsmos.h>
#include "sleep.h"

// 10 seconds
#define IDLE_TIMEOUT (15000U)

static volatile unsigned int _last_ms;

void sleep_init() {
    sleep_reset();

    INT_CLKO |= 0x40;
}

void sleep_reset() {
    _last_ms = millis();
}

void sleep_check() {
    if ((millis() - _last_ms) > IDLE_TIMEOUT) {
        // put to sleep mode
        PCON = 0x02;

        // Waked up
        __asm nop __endasm;
        __asm nop __endasm;
        __asm nop __endasm;
        __asm nop __endasm;

        sleep_reset();
    }
}