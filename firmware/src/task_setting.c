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
#include "charsdef.h"
#include "version.h"
#include "profiles.h"

static unsigned char callsign_pos;

static __code char version[] = {
    MCH_SP,
    KEYER_VERSION,

#if !defined(COMPACT_CODE)
    MCH_F,
#endif

    MCH_SLASH,

#if KEYING_ON == 1
    MCH_P,
#else
    MCH_N,
#endif

#if TXING_ON == 1
    MCH_P,
#else
    MCH_N,
#endif

#if TXING_AS_MUTE == 1
    MCH_M,
#else
    MCH_T,
#endif

    0
};

static __code char resp_ok[] = { MCH_SP, MCH_O, MCH_K, 0 }; // " OK"
static __code char resp_sk[] = { MCH_S, MCH_K, 0 }; // "SK"

static void do_wait_command() {
    if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        if (CFG_FLAG_LOCKED && ch != MCH_5 && ch != MCH_V && ch != MCH_LF) {
            autotext_response_char2(MCH_EM);
            continue;
        }

        switch (ch) {
            case MCH_LF: // exit setting mode
                save_config();
                autotext_response_str(resp_sk);
                RT_FLAG_DISABLE_SETTING_MODE;
                break;

            case MCH_0: // reset config
                cfg_reset();
                autotext_response_str(resp_ok);
                break;

            case MCH_V:
                autotext_response_str(version);
                break;

            case MCH_W: // query wpm
                autotext_response_num(_config.speed_wpm);
                break;
            case MCH_E: // decrease wpm
            case MCH_T: // increase wpm
                cfg_set_speed_wpm(_config.speed_wpm + (ch == MCH_T ? 1 : -1));
                autotext_response_num(_config.speed_wpm);
                break;

            case MCH_A: // set iambic A
                CFG_FLAG_SET_IAMBIC_A;
                autotext_response_str(resp_ok);
                break;
            case MCH_B: // set iambic B
                CFG_FLAG_SET_IAMBIC_B;
                autotext_response_str(resp_ok);
                break;

            case MCH_L:
                CFG_FLAG_SET_LEFT_HAND;
                autotext_response_str(resp_ok);
                break;
            case MCH_R:
                CFG_FLAG_SET_RIGHT_HAND;
                autotext_response_str(resp_ok);
                break;

            case MCH_F: // set sidetone frequency
                autotext_response_num(times_50(_config.sidetone_freq_50hz));
                tfo_goto_state(TASK_SETTING, SETTING_STATE_SIDETONE_FREQ);
                break;

            case MCH_D: // set tx delay
                autotext_response_num(times_50(_config.tx_delay_50ms));
                tfo_goto_state(TASK_SETTING, SETTING_STATE_TX_DELAY);
                break;

            case MCH_C: // set callsign
                autotext_response_char2(MCH_QM);
                rt_morse_decoded_buffer_reset();
                callsign_pos = 0;
                tfo_goto_state(TASK_SETTING, SETTING_STATE_CALLSIGN);
                break;

            case MCH_K: // toggle tx keying
                CFG_FLAG_TOGGLE_TX;
                autotext_response_char2(CFG_FLAG_TX_ENABLED ? MCH_Y : MCH_N);
                break;

            case MCH_S: // toggle sidetone
                CFG_FLAG_TOGGLE_SIDETONE;
                autotext_response_char2(CFG_FLAG_SIDETONE_ENABLED ? MCH_Y : MCH_N);
                break;

            case MCH_O: // toggle loop
                CFG_FLAG_TOGGLE_LOOP;
                autotext_response_char2(CFG_FLAG_LOOP_ENABLED ? MCH_Y : MCH_N);
                break;

            case MCH_5: // toggle config lock
                CFG_FLAG_TOGGLE_LOCK;
                autotext_response_char2(CFG_FLAG_LOCKED ? MCH_Y : MCH_N);
                break;
        }
    }
}

static void do_sidetone_freq() {
    // In this state, setting mode is always enabled.
    // if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        switch (ch) {
            case MCH_LF: // return to main setting
                autotext_response_str(resp_ok + 1);
                tfo_goto_state(TASK_SETTING, SETTING_STATE_WAIT_COMMAND);
                break;
            case MCH_T:
            case MCH_E:
                cfg_set_sidetone_freq_50hz(_config.sidetone_freq_50hz + (ch == MCH_T ? 1 : -1));
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
            case MCH_LF: // return to main setting
                autotext_response_str(resp_ok + 1);
                tfo_goto_state(TASK_SETTING, SETTING_STATE_WAIT_COMMAND);
                break;
            case MCH_T:
            case MCH_E:
                cfg_set_tx_delay_50ms(_config.tx_delay_50ms + (ch == MCH_T ? 1 : -1));
                autotext_response_num(times_50(_config.tx_delay_50ms));
                break;
        }
    }
}

static void do_callsign() {
    if (!RT_FLAG_SETTING_MODE_ENABLED) return;

    for (char ch = rt_morse_decoded_buffer_getch(); ch != 0; ch = rt_morse_decoded_buffer_getch()) {
        switch (ch) {
            case MCH_LF: // return to main setting
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
