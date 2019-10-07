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

static unsigned char callsign_pos;

static __code char version[] = VERSION;
static __code char resp_ok[] = " OK";
static __code char resp_sk[] = "SK";
static __code char resp_question_mark[] = " ?";

static void do_wait_command() {
    if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        switch (ch) {
            case '\n': // exit setting mode
                save_config();
                autotext_response_str(resp_sk);
                RT_FLAG_DISABLE_SETTING_MODE;
                break;

            case '0': // reset config
                cfg_reset();
                autotext_response_str(resp_ok);
                break;

            case 'V':
                autotext_response_str(version);
                break;

            case 'W': // query wpm
                autotext_response_num(_config.speed_wpm);
                break;
            case 'E': // decrease wpm
            case 'T': // increase wpm
                cfg_set_speed_wpm(_config.speed_wpm + (ch == 'T' ? 1 : -1));
                autotext_response_num(_config.speed_wpm);
                break;

            case 'A': // set iambic A
                CFG_FLAG_SET_IAMBIC_A;
                autotext_response_str(resp_ok);
                break;
            case 'B': // set iambic B
                CFG_FLAG_SET_IAMBIC_B;
                autotext_response_str(resp_ok);
                break;

            case 'L':
                CFG_FLAG_SET_LEFT_HAND;
                autotext_response_str(resp_ok);
                break;
            case 'R':
                CFG_FLAG_SET_RIGHT_HAND;
                autotext_response_str(resp_ok);
                break;

            case 'F': // set sidetone frequency
                autotext_response_num(times_50(_config.sidetone_freq_50hz));
                tfo_goto_state(TASK_SETTING, SETTING_STATE_SIDETONE_FREQ);
                break;

            case 'D': // set tx delay
                autotext_response_num(times_50(_config.tx_delay_50ms));
                tfo_goto_state(TASK_SETTING, SETTING_STATE_TX_DELAY);
                break;

            case 'C': // set callsign
                autotext_response_str(resp_question_mark);
                rt_morse_decoded_buffer_reset();
                callsign_pos = 0;
                tfo_goto_state(TASK_SETTING, SETTING_STATE_CALLSIGN);
                break;
        }
    }
}

static void do_sidetone_freq() {
    // In this state, setting mode is always enabled.
    // if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        switch (ch) {
            case '\n': // return to main setting
                autotext_response_str(resp_ok + 1);
                tfo_goto_state(TASK_SETTING, SETTING_STATE_WAIT_COMMAND);
                break;
            case 'T':
            case 'E':
                cfg_set_sidetone_freq_50hz(_config.sidetone_freq_50hz + (ch == 'T' ? 1 : -1));
                autotext_response_num(times_50(_config.sidetone_freq_50hz));
                break;
        }
    }
}

static void do_tx_delay() {
    // In this state, setting mode is always enabled.
    // if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        switch (ch) {
            case '\n': // return to main setting
                autotext_response_str(resp_ok + 1);
                tfo_goto_state(TASK_SETTING, SETTING_STATE_WAIT_COMMAND);
                break;
            case 'T':
            case 'E':
                cfg_set_tx_delay_50ms(_config.tx_delay_50ms + (ch == 'T' ? 1 : -1));
                autotext_response_num(times_50(_config.tx_delay_50ms));
                break;
        }
    }
}

static void do_callsign() {
    if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        switch (ch) {
            case '\n': // return to main setting
                if (callsign_pos) {
                    _config.callsign[callsign_pos] = 0;
                }
                autotext_response_str(_config.callsign);
                tfo_goto_state(TASK_SETTING, SETTING_STATE_WAIT_COMMAND);
                break;

            default:
                _config.callsign[callsign_pos] = ch;
                if (callsign_pos < (CFG_CALLSIGN_BUFFER_LEN - 1)) callsign_pos ++;
                break;
        }
    }
}

void setting_state_machine() {
    tfo_task_state state = tfo_get_task_state(TASK_SETTING);

    // No delay. No in-state. Ingore state flag check.
    // if (TFO_STATE_FLAGS(state)) return;

    switch (state) {
        case TFO_STATE_INIT:
            tfo_goto_state(TASK_SETTING, SETTING_STATE_WAIT_COMMAND);
            break;
        case SETTING_STATE_WAIT_COMMAND:
            do_wait_command();
            break;
        case SETTING_STATE_SIDETONE_FREQ:
            do_sidetone_freq();
            break;
        case SETTING_STATE_TX_DELAY:
            do_tx_delay();
            break;
        case SETTING_STATE_CALLSIGN:
            do_callsign();
            break;
    }
}