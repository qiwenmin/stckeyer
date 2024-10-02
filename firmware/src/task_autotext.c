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
#include "charsdef.h"

#define CODE_DAH (0b10000000)

static unsigned char char_pattern;
static tfo_task_state send_chars_return_state;
static unsigned char autocq_idx;
static __code char msg_cq1[] = { MCH_C, MCH_Q, MCH_SP, MCH_C, MCH_Q, MCH_SP, MCH_D, MCH_E, MCH_SP, 0 }; // "CQ CQ DE "
static __code char msg_cq2[] = { MCH_SP, 0 }; // " "
static __code char msg_cq3[] = { MCH_SP, MCH_P, MCH_S, MCH_E, MCH_SP, MCH_K, 0}; // " PSE K"

static void goto_state_idle(void) {
    tfo_goto_state(TASK_KEYING, KEYING_STATE_UP);

    RT_FLAG_ENABLE_PADDLE;
    RT_FLAG_ENABLE_SW;

    if (RT_FLAG_SETTING_MODE_ENABLED) {
        RT_FLAG_DISABLE_TX;
    } else {
        RT_FLAG_ENABLE_TX;
    }

    RT_FLAG_DISABLE_AUTOCQ;
    RT_FLAG_SET_RESPONSING_OFF;

    char_pattern = MORSE_PATTERN_FINISHED;
    _runtime_autotext_ptr = 0;

    autocq_idx = 0;
}

static void goto_state_autocq(void) {
    if (autocq_idx == 0) {
        RT_FLAG_DISABLE_PADDLE;
        RT_FLAG_ENABLE_AUTOCQ;

        send_chars_return_state = AUTOTEXT_STATE_AUTOCQ;

        _runtime_autotext_ptr = msg_cq1;
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CHARS);
        autocq_idx = 1;
    } else if (autocq_idx == 1) {
        _runtime_autotext_ptr = _config.callsign;
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CHARS);
        autocq_idx = 2;
    } else if (autocq_idx == 2) {
        _runtime_autotext_ptr = msg_cq2;
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CHARS);
        autocq_idx = 3;
    } else if (autocq_idx == 3) {
        _runtime_autotext_ptr = _config.callsign;
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CHARS);
        autocq_idx = 4;
    } else if (autocq_idx == 4) {
        _runtime_autotext_ptr = msg_cq3;
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CHARS);
        autocq_idx = 5;
    } else {
        autocq_idx = 0;
        if (CFG_FLAG_LOOP_ENABLED) {
            tfo_delay(TASK_AUTOTEXT, 8000, AUTOTEXT_STATE_AUTOCQ);
        } else {
            tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_IDLE);
        }
    }
}

static void goto_state_response(void) {
    RT_FLAG_DISABLE_PADDLE;
    RT_FLAG_DISABLE_SW;
    RT_FLAG_DISABLE_TX;
    RT_FLAG_SET_RESPONSING_ON;

    send_chars_return_state = AUTOTEXT_STATE_IDLE;
    tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CHARS);
}

static void goto_state_send_chars(void) {
    if (_runtime_autotext_ptr == 0 || (*_runtime_autotext_ptr == 0)) {
        tfo_goto_state(TASK_AUTOTEXT, send_chars_return_state);

        return;
    }

    char_pattern = morse_get_pattern(*_runtime_autotext_ptr);
    _runtime_autotext_ptr ++;

    if (char_pattern == MORSE_PATTERN_FINISHED) {
        tfo_delay(TASK_AUTOTEXT, _runtime_cw_ems * 4, AUTOTEXT_STATE_SEND_CHARS);
    } else {
        tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_SEND_CODE);
    }
}

void autotext_state_machine(void) {
    tfo_task_state state = tfo_get_task_state(TASK_AUTOTEXT);

    if (TFO_STATE_FLAGS(state)) return;

    switch (state) {
        case TFO_STATE_INIT:
            tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_IDLE);
            break;
        case AUTOTEXT_STATE_IDLE:
            goto_state_idle();
            tfo_in_state(TASK_AUTOTEXT);
            break;
        case AUTOTEXT_STATE_AUTOCQ:
            goto_state_autocq();
            break;
        case AUTOTEXT_STATE_RESPONSE:
            goto_state_response();
            break;
        case AUTOTEXT_STATE_SEND_CHARS:
            goto_state_send_chars();
            break;
        case AUTOTEXT_STATE_SEND_CODE:
            if (char_pattern == MORSE_PATTERN_FINISHED) {
                tfo_delay(TASK_AUTOTEXT, _runtime_cw_ems * 2, AUTOTEXT_STATE_SEND_CHARS);
            } else {
                tfo_goto_state(TASK_AUTOTEXT, char_pattern & CODE_DAH ? AUTOTEXT_STATE_DAH : AUTOTEXT_STATE_DIT);
                char_pattern = char_pattern << 1;
            }
            break;
        case AUTOTEXT_STATE_DIT:
        case AUTOTEXT_STATE_DAH:
            tfo_goto_state(TASK_KEYING, KEYING_STATE_DOWN);
            tfo_delay(TASK_AUTOTEXT,
                state == AUTOTEXT_STATE_DIT ? _runtime_cw_ems : _runtime_cw_ems * 3,
                AUTOTEXT_STATE_IEG);
            break;
        case AUTOTEXT_STATE_IEG:
            tfo_goto_state(TASK_KEYING, KEYING_STATE_UP);
            tfo_delay(TASK_AUTOTEXT, _runtime_cw_ems, AUTOTEXT_STATE_SEND_CODE);
    }
}

void autotext_response_str(char *p) {
    _runtime_autotext_ptr = p;
    tfo_goto_state(TASK_AUTOTEXT, AUTOTEXT_STATE_RESPONSE);
}

static void resp_buffer_fill_number(unsigned int n) {
    unsigned char i = 0;

    _runtime_resp_buffer[i ++] = MCH_SP;

    unsigned int base = 1000;

    while (base != 1) {
        if ((i > 1) || (n >= base)) {
            _runtime_resp_buffer[i ++] = NUM2CH(divide(n, base));
        }
        n = n % base;
        base = divide(base, 10);
    }

    _runtime_resp_buffer[i ++] = NUM2CH(n);
    _runtime_resp_buffer[i ++] = 0;
}

void autotext_response_num(unsigned int n) {
    resp_buffer_fill_number(n);
    autotext_response_str(_runtime_resp_buffer);
}

void autotext_response_char(char ch) {
    _runtime_resp_buffer[0] = ch;
    _runtime_resp_buffer[1] = 0;
    autotext_response_str(_runtime_resp_buffer);
}

void autotext_response_char2(char ch) {
    _runtime_resp_buffer[0] = MCH_SP;
    _runtime_resp_buffer[1] = ch;
    _runtime_resp_buffer[2] = 0;
    autotext_response_str(_runtime_resp_buffer);
}
