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
#include "morse.h"
#include "hw.h"
#include "charsdef.h"
#include "sleep.h"

#define PADDLE_NONE (0)
#define PADDLE_DIT (1)
#define PADDLE_DAH (2)
#define PADDLE_BOTH (PADDLE_DIT | PADDLE_DAH)

static unsigned char _current_paddle_key, _next_paddle_key;
static unsigned char _morse_bits;

#define init_paddle() {\
    DIT_PIN = 1;\
    DAH_PIN = 1;\
    _current_paddle_key = _next_paddle_key = PADDLE_NONE;\
    _morse_bits = 1;\
}

#define is_raw_dit_down() (!DIT_PIN)
#define is_dit_pin_down() (RT_FLAG_PADDLE_ENABLED && is_raw_dit_down())

#define is_raw_dah_down() (!DAH_PIN)
#define is_dah_pin_down() (RT_FLAG_PADDLE_ENABLED && is_raw_dah_down())

static unsigned char get_paddle_key() {
    unsigned char result = (is_dit_pin_down() ? PADDLE_DIT : PADDLE_NONE)
        | (is_dah_pin_down() ? PADDLE_DAH : PADDLE_NONE);

    if (CFG_FLAG_IS_LEFT_HAND) {
        result = ((result & 0x01) << 1) | ((result & 0x02) >> 1);
    }

    return result;
}

static void key_down() {
    tfo_goto_state(TASK_KEYING, KEYING_STATE_DOWN);
}

static void key_up() {
    tfo_goto_state(TASK_KEYING, KEYING_STATE_UP);
}

void paddle_state_machine() {
    if (RT_FLAG_AUTOCQ_ENABLED && (
        is_raw_dit_down() || ((!RT_FLAG_STRAIGHT_KEY_ENABLED) && is_raw_dah_down())
    )) {
        tfo_goto_state_force(TASK_AUTOTEXT, AUTOTEXT_STATE_IDLE);
    }

    tfo_task_state state = tfo_get_task_state(TASK_PADDLE);

    if (TFO_STATE_FLAGS(state)) {
        if ((TFO_STATE_IS_WATING_FOR(state, PADDLE_STATE_IEG) || TFO_STATE_IS_WATING_FOR(state, PADDLE_STATE_IDLE))
            && _current_paddle_key != PADDLE_NONE
            && _next_paddle_key == PADDLE_NONE) {
            _next_paddle_key = get_paddle_key() & (~_current_paddle_key);
        }

        return;
    }

    switch (state) {
        case TFO_STATE_INIT:
            init_paddle();

            if (is_dah_pin_down()) {
                RT_FLAG_ENABLE_STRAIGHT_KEY;
            }
            tfo_goto_state(TASK_PADDLE, PADDLE_STATE_REPORT_KEY_TYPE);

            break;

        case PADDLE_STATE_REPORT_KEY_TYPE:
            if (RT_FLAG_STRAIGHT_KEY_ENABLED) {
                autotext_response_char(MCH_S);
                tfo_goto_state(TASK_PADDLE, PADDLE_STATE_STRAIGHT_KEY_WAITING_DOWN);
            } else {
                autotext_response_char(MCH_P);
                tfo_goto_state(TASK_PADDLE, PADDLE_STATE_IDLE);
            }
            break;

        // ******** Straight Key ********
        case PADDLE_STATE_STRAIGHT_KEY_WAITING_DOWN:
            if (is_dit_pin_down()) {
                key_down();
                tfo_delay(TASK_PADDLE, 50, PADDLE_STATE_STRAIGHT_KEY_WAITING_UP);
            }
            break;

        case PADDLE_STATE_STRAIGHT_KEY_WAITING_UP:
            if (!is_dit_pin_down()) {
                key_up();
                tfo_delay(TASK_PADDLE, 50, PADDLE_STATE_STRAIGHT_KEY_WAITING_DOWN);
            } else {
                sleep_reset();
            }
            break;

        // ******** Paddle ********
        case PADDLE_STATE_IDLE:
            if (_next_paddle_key != PADDLE_NONE) {
                _current_paddle_key = _next_paddle_key;
                _next_paddle_key = PADDLE_NONE;
            } else {
                _current_paddle_key = get_paddle_key();
                if (_current_paddle_key == PADDLE_BOTH) {
                    _current_paddle_key = PADDLE_DAH;
                    _next_paddle_key = PADDLE_DIT;
                }
            }

            if (_current_paddle_key == PADDLE_DAH) {
                key_down();
                tfo_goto_state(TASK_PADDLE, PADDLE_STATE_DAH);
            } else if (_current_paddle_key == PADDLE_DIT) {
                key_down();
                tfo_goto_state(TASK_PADDLE, PADDLE_STATE_DIT);
            } else {
                // decode morse code
                if (_morse_bits != 0x01) {
                    if (_morse_bits & 0x80) { // maybe '$'
                        _morse_bits = (_morse_bits << 1) | 0x01;
                    } else {
                        _morse_bits = (_morse_bits << 1) | 0x01;
                        while ((_morse_bits & 0x80) == 0) {
                            _morse_bits <<= 1;
                        }

                        _morse_bits <<= 1;
                    }

                    char ch = morse_get_char(_morse_bits);
                    _morse_bits = 0x01;
                    if (ch != 0) {
                        // morse code decoded!
                        rt_morse_decoded_buffer_putch(ch);
                    }
                }
            }
            break;

        case PADDLE_STATE_DAH:
            _morse_bits = (_morse_bits << 1) | 0x01;
            tfo_delay(TASK_PADDLE, _runtime_cw_ems * 3, PADDLE_STATE_IEG);
            break;

        case PADDLE_STATE_DIT:
            _morse_bits = (_morse_bits << 1);
            tfo_delay(TASK_PADDLE, _runtime_cw_ems, PADDLE_STATE_IEG);
            break;

        case PADDLE_STATE_IEG:
            key_up();
            if (CFG_FLAG_IS_IAMBIC_A) {
                if (get_paddle_key() == PADDLE_NONE) {
                    _current_paddle_key = _next_paddle_key = PADDLE_NONE;
                }
            }
            tfo_delay(TASK_PADDLE, _runtime_cw_ems, PADDLE_STATE_IDLE);
            break;
    }
}
