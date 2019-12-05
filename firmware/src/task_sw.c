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
#include "hw.h"
#include "charsdef.h"

#define init_sw() SW_PIN = 1

#define is_pin_down() (RT_FLAG_SW_ENABLED && (!SW_PIN))

static void do_long_press() {
    if (RT_FLAG_SETTING_MODE_ENABLED || RT_FLAG_AUTOCQ_ENABLED) return;

    RT_FLAG_DISABLE_TX;
    RT_FLAG_ENABLE_SETTING_MODE;

    rt_morse_decoded_buffer_reset();

    autotext_response_char(MCH_QM);
}

static void do_press() {
    if (RT_FLAG_SETTING_MODE_ENABLED) {
        rt_morse_decoded_buffer_putch(MCH_LF);
    } else if (RT_FLAG_AUTOCQ_ENABLED) {
        tfo_goto_state_force(TASK_AUTOTEXT, AUTOTEXT_STATE_IDLE);
    } else {
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_AUTOCQ);
    }
}

void sw_state_machine() {
    tfo_task_state state = tfo_get_task_state(TASK_SW);

    if (TFO_STATE_FLAGS(state)) {
        if (TFO_STATE_IS_WATING_FOR(state, SW_STATE_WAITING_LONG_UP)) {
            if (!is_pin_down()) {
                do_press();
                tfo_delay_force(TASK_SW, 50, SW_STATE_WAITING_DOWN);
            }
        }
        return;
    }

    switch (state) {
        case TFO_STATE_INIT:
            init_sw();
            tfo_goto_state(TASK_SW, SW_STATE_WAITING_DOWN);
            break;
        case SW_STATE_WAITING_DOWN:
            if (is_pin_down()) {
                tfo_delay(TASK_SW, 2000, SW_STATE_WAITING_LONG_UP);
            }
            break;
        case SW_STATE_WAITING_LONG_UP:
            do_long_press();
            tfo_goto_state(TASK_SW, SW_STATE_WAITING_UP);
            break;
        case SW_STATE_WAITING_UP:
            if (!is_pin_down()) {
                tfo_delay(TASK_SW, 50, SW_STATE_WAITING_DOWN);
            }
            break;
    }
}
