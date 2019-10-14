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

#include "tnyfsmos/tnyfsmos.h"
#include "tasksdef.h"
#include "config.h"

// keying task
static void keying_init();
static void keying_up();
static void keying_down();

void keying_state_machine() {
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
            RT_FLAG_SET_SIDETONE_ON;

            if (RT_FLAG_TX_ENABLED) {
                keying_down();
                tfo_goto_state_force(TASK_TXING, TXING_STATE_ON);
            }

            tfo_in_state(TASK_KEYING);
            break;
    }
}

// txing task
static void txing_init();
static void txing_off();
static void txing_on();

void txing_state_machine() {
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

/*********************************************************************
 * MCS51
 *********************************************************************/
#ifdef __SDCC_mcs51

#define KEY_PIN P3_5
#define INIT_KEY_PIN P3M0 |= (1 << 5)

#define TX_PIN P3_2
#define INIT_TX_PIN P3M0 |= (1 << 2)

#define SIDETONE_PIN P3_3
#define INIT_SIDETONE_PIN P3M0 |= (1 << 3)

#define FOSC 6000000

void update_timer0() {
	ET0 = 0; // Disable Timer0 interrupt
	TR0 = 0; // Stop Timer0

    unsigned int v = 0x10000 - divide(FOSC / 12 / 2 / 50, _config.sidetone_freq_50hz);

	TL0 = v;
    TH0 = v >> 8;

	TF0 = 0; // Clear TF0 flag

	TR0 = 1; // Start Timer0
	ET0 = 1; // Enable Timer0 interrupt
}

static void init_timer0() {
    PT0 = 1; // Set Timer0 High Priority

    AUXR &= ~0x80; // Timer clock is 12T mode
	TMOD &= 0xF0; // Set timer work mode

    // updated in config resetting
    //update_timer0();
}

void timer0_isr() __interrupt TF0_VECTOR {
    if (RT_FLAG_IS_SIDETONE_ON) {
        SIDETONE_PIN = (!SIDETONE_PIN) & 0x01;
    } else {
        SIDETONE_PIN = 0;
    }
}

static void keying_init() {
    INIT_KEY_PIN;
    INIT_SIDETONE_PIN;
    init_timer0();
}

static void keying_down() {
    KEY_PIN = 1;
}

static void keying_up() {
    KEY_PIN = 0;
}

static void txing_init() {
    INIT_TX_PIN;
}

static void txing_on() {
    TX_PIN = 1;
}

static void txing_off() {
    TX_PIN = 0;
}

#endif // __SDCC_mcs51

/*********************************************************************
 * STM8
 *********************************************************************/

// TODO - not implemented
/*#ifdef __SDCC_stm8

#include "stm8s.h"
#include "stm8util.h"

void init_led() {
    set_bit(PB_DDR, 5);
    set_bit(PB_CR1, 5);
}

void led_on() {
    clear_bit(PB_ODR, 5);
}

void led_off() {
    set_bit(PB_ODR, 5);
}

#endif // __SDCC_stm8
*/
