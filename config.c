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

#include "config.h"
#include "tnyfsmos/stc15.h"

config _config;

unsigned char _runtime_cw_ems;
signed char _runtime_flags;
char const *_runtime_autotext_ptr;
char _runtime_resp_buffer[6];

static unsigned char wpm2ems(unsigned char wpm) {
    return 1200 / wpm;
}

void cfg_set_speed_wpm(unsigned char value) {
    if (value < CFG_SPEED_WPM_MIN || value > CFG_SPEED_WPM_MAX) return;

    _config.speed_wpm = value;
    _runtime_cw_ems = wpm2ems(_config.speed_wpm);
}

void update_timer0(); // for update sidetone frequency

void cfg_set_sidetone_freq_50hz(unsigned char value) {
    if (value < CFG_SIDETONE_FREQ_50HZ_MIN || value > CFG_SIDETONE_FREQ_50HZ_MAX) return;

    _config.sidetone_freq_50hz = value;

    update_timer0();
}

void cfg_set_tx_delay_50ms(unsigned char value) {
    if (/*value < CFG_TX_DELAY_50MS_MIN ||*/ value > CFG_TX_DELAY_50MS_MAX) return;

    _config.tx_delay_50ms = value;
}

void cfg_reset() {
    _config.tx_delay_50ms = 12; //  600ms
    _config.speed_wpm = 15;
    _config.sidetone_freq_50hz = 14; // 700Hz

    CFG_FLAG_SET_DEFAULTS;

    _config.callsign[0] = 'N';
    _config.callsign[1] = 'N';
    _config.callsign[2] = 'K';
    _config.callsign[3] = 0;

    _runtime_cw_ems = wpm2ems(_config.speed_wpm);
    update_timer0();
}

void reset_config_and_runtime_values() {
    cfg_reset();

    // reset the the runtime variables
    RT_FLAG_SET_DEFAULTS;

    _runtime_autotext_ptr = 0;
}

unsigned int times_50(unsigned char v) {
    return 50 * v;
}

unsigned int divide(unsigned int a, unsigned int b) {
    return a / b;
}

// decoded buffer
#define MD_BUF_LEN_BITS (2)
#define MD_BUF_LEN (1 << MD_BUF_LEN_BITS)
static char md_buf[MD_BUF_LEN];
unsigned char md_buf_pos, md_buf_size;

void rt_morse_decoded_buffer_reset() {
    md_buf_pos = md_buf_size = 0;
}

char rt_morse_decoded_buffer_getch() {
    if (md_buf_size == 0) {
        return 0;
    }

    char result = md_buf[(md_buf_pos - (md_buf_size --)) & (MD_BUF_LEN - 1)];
    return result;
}

void rt_morse_decoded_buffer_putch(char ch) {
    md_buf[md_buf_pos ++] = ch;
    md_buf_pos &= (MD_BUF_LEN - 1);

    if (md_buf_size < MD_BUF_LEN) md_buf_size ++;
}

// eeprom operations
static void iap_idle() {
    IAP_CONTR = 0;
    IAP_CMD = 0;
    IAP_TRIG = 0;
    IAP_ADDRH = 0x80;
    IAP_ADDRL = 0;
}

#define ENABLE_IAP 0x84 // < 6MHz
#define CMD_READ 1 //IAP Byte-Read
#define CMD_PROGRAM 2 //IAP Byte-Program
#define CMD_ERASE 3 //IAP Sector-Erase

static void iap_erase_sector(unsigned int addr) {
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_ERASE;
    IAP_ADDRL = addr;
    IAP_ADDRH = addr >> 8;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    __asm nop __endasm;
    iap_idle();
}

static unsigned char iap_read_byte(unsigned int addr) {
    unsigned char dat;
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_READ;
    IAP_ADDRL = addr;
    IAP_ADDRH = addr >> 8;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    __asm nop __endasm;
    dat = IAP_DATA;
    iap_idle();
    return dat;
}

static void  iap_program_byte(unsigned int addr, unsigned char dat)
{
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_PROGRAM;
    IAP_ADDRL = addr;
    IAP_ADDRH = addr>>8;
    IAP_DATA = dat;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    __asm nop __endasm;
    iap_idle();
}

void load_config() {
    unsigned char ch1 = iap_read_byte(sizeof(_config));

    if (ch1 != 'N') return;

    unsigned char *p = (unsigned char *)(&_config);
    for (unsigned char i = 0; i < sizeof(_config); i ++) {
        p[i] = iap_read_byte(i);
    }

    _runtime_cw_ems = wpm2ems(_config.speed_wpm);
}

void save_config() {
    iap_erase_sector(0x00);

    iap_program_byte(sizeof(_config), 'N');

    unsigned char *p = (unsigned char *)(&_config);
    for (unsigned char i = 0; i < sizeof(_config); i ++) {
        iap_program_byte(i, p[i]);
    }
}