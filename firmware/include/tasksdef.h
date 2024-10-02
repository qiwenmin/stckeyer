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

#ifndef __TASKSDEF_H__
#define __TASKSDEF_H__

typedef enum {
    TASK_PADDLE = 0,
    TASK_SW,

    TASK_KEYING,
    TASK_AUTOTEXT,
    TASK_TXING,
    TASK_SETTING,

    TASK_COUNT
};

/*****************
 * SW Task
 *****************/
typedef enum {
    SW_STATE_WAITING_UP = TFO_STATE_USER,
    SW_STATE_DEBOUNING_DOWN,
    SW_STATE_WAITING_LONG_UP,
    SW_STATE_WAITING_DOWN
};

void sw_state_machine(void);

/*****************
 * Paddle Task
 *****************/
typedef enum {
    PADDLE_STATE_REPORT_KEY_TYPE = TFO_STATE_USER,
    PADDLE_STATE_STRAIGHT_KEY_WAITING_DOWN,
    PADDLE_STATE_STRAIGHT_KEY_WAITING_UP,

    PADDLE_STATE_IDLE,
    PADDLE_STATE_DAH,
    PADDLE_STATE_DIT,
    PADDLE_STATE_IEG
};

void paddle_state_machine(void);

/*****************
 * Keying Task
 *****************/
typedef enum {
    KEYING_STATE_UP = TFO_STATE_USER,
    KEYING_STATE_DOWN
};

void keying_state_machine(void);
void update_timer0(void);

/*****************
 * TXing Task
 *****************/
typedef enum {
    TXING_STATE_OFF = TFO_STATE_USER,
    TXING_STATE_ON
};

void txing_state_machine(void);

/*****************
 * AutoText Task
 *****************/
typedef enum {
    AUTOTEXT_STATE_IDLE = TFO_STATE_USER,
    AUTOTEXT_STATE_AUTOCQ,
    AUTOTEXT_STATE_RESPONSE,
    AUTOTEXT_STATE_SEND_CHARS,

    AUTOTEXT_STATE_SEND_CODE,

    AUTOTEXT_STATE_DIT,
    AUTOTEXT_STATE_DAH,
    AUTOTEXT_STATE_IEG
};

void autotext_state_machine(void);
void autotext_response_str(char *p);
void autotext_response_num(unsigned int n);
void autotext_response_char(char ch);
void autotext_response_char2(char ch);

/*****************
 * Setting Task
 *****************/
typedef enum {
    SETTING_STATE_IDLE = TFO_STATE_USER,
    SETTING_STATE_WAIT_COMMAND,

    SETTING_STATE_SIDETONE_FREQ,
    SETTING_STATE_TX_DELAY,
    SETTING_STATE_CALLSIGN
};

void setting_state_machine(void);

#endif // __TASKSDEF_H__
