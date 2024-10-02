/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <tnyfsmos.h>
#include "tasksdef.h"
#include "config.h"
#include "profiles.h"
#include "sleep.h"

// keying task
#ifdef STC8G1K08
#define KEY_PIN P5_5
#define INIT_KEY_PIN P5M0 |= (1 << 5); P5M1 &= ~(1 << 5)
#else
#define KEY_PIN P3_5
#define INIT_KEY_PIN P3M0 |= (1 << 5); P3M1 &= ~(1 << 5)
#endif // STC8G1K08

#define TX_PIN P3_2
#define INIT_TX_PIN P3M0 |= (1 << 2); P3M1 &= ~(1 << 2)

#define SIDETONE_PIN P3_3
#define INIT_SIDETONE_PIN P3M0 |= (1 << 3); P3M1 &= ~(1 << 3)

#define FOSC 6000000

void update_timer0(void) {
	ET0 = 0; // Disable Timer0 interrupt
	TR0 = 0; // Stop Timer0

    unsigned int v = 0x10000 - divide(FOSC / 12 / 2 / 50, _config.sidetone_freq_50hz);

	TL0 = v;
    TH0 = v >> 8;

	TF0 = 0; // Clear TF0 flag

	TR0 = 1; // Start Timer0
	ET0 = 1; // Enable Timer0 interrupt
}

void timer0_isr(void) __interrupt (TF0_VECTOR) {
    if ((CFG_FLAG_SIDETONE_ENABLED || RT_FLAG_IS_RESPONSING_ON || RT_FLAG_SETTING_MODE_ENABLED)
        && RT_FLAG_IS_SIDETONE_ON) {
        SIDETONE_PIN = (!SIDETONE_PIN) & 0x01;
    } else {
        SIDETONE_PIN = 0;
    }
}

#define init_timer0() {\
    PT0 = 1; /* Set Timer0 High Priority */\
    AUXR &= ~0x80; /* Timer clock is 12T mode */\
    TMOD &= 0xF0; /* Set timer work mode */\
\
    /* updated in config resetting */\
    /* update_timer0(); */\
}

#define keying_down() KEY_PIN = KEYING_ON

#define keying_up() KEY_PIN = KEYING_OFF

#define txing_on() TX_PIN = TXING_ON

#define txing_off() TX_PIN = TXING_OFF

#define txing_init() {\
    INIT_TX_PIN;\
    txing_off();\
}

#define keying_init() {\
    INIT_KEY_PIN;\
    INIT_SIDETONE_PIN;\
    keying_up();\
    init_timer0();\
}

void keying_state_machine(void) {
    tfo_task_state state = tfo_get_task_state(TASK_KEYING);

    if (TFO_STATE_FLAGS(state)) return;

    switch (state) {
        case TFO_STATE_INIT:
            keying_init();
            tfo_goto_state(TASK_KEYING, KEYING_STATE_UP);
            break;
        case KEYING_STATE_UP:
            RT_FLAG_SET_SIDETONE_OFF;
            keying_up();
            tfo_delay_force(TASK_TXING, times_50(_config.tx_delay_50ms), TXING_STATE_OFF);
            tfo_in_state(TASK_KEYING);
            break;
        case KEYING_STATE_DOWN:
            sleep_reset();
            RT_FLAG_SET_SIDETONE_ON;
#if TXING_AS_MUTE == 1
            tfo_goto_state_force(TASK_TXING, TXING_STATE_ON);
#endif
            if (CFG_FLAG_TX_ENABLED && RT_FLAG_TX_ENABLED) {
                keying_down();
#if TXING_AS_MUTE != 1
                tfo_goto_state_force(TASK_TXING, TXING_STATE_ON);
#endif
            }

            tfo_in_state(TASK_KEYING);
            break;
    }
}

void txing_state_machine(void) {
    tfo_task_state state = tfo_get_task_state(TASK_TXING);

    if (TFO_STATE_FLAGS(state)) return;

    switch (state) {
        case TFO_STATE_INIT:
            txing_init();
            tfo_goto_state(TASK_TXING, TXING_STATE_OFF);
            break;
        case TXING_STATE_OFF:
            txing_off();
            tfo_in_state(TASK_TXING);
            break;
        case TXING_STATE_ON:
            txing_on();
            tfo_in_state(TASK_TXING);
            break;
    }
}
