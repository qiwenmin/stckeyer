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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VERSION " 002P2 3p7p BD4KG"

#define CFG_CALLSIGN_BUFFER_LEN (12)

#define CFG_SIDETONE_FREQ_50HZ_MIN (8)
#define CFG_SIDETONE_FREQ_50HZ_MAX (30)

#define CFG_SPEED_WPM_MIN (5)
#define CFG_SPEED_WPM_MAX (50)

#define CFG_TX_DELAY_50MS_MIN (0)
#define CFG_TX_DELAY_50MS_MAX (40)

typedef struct {
    // result = v * 50
    // v: 8 - 30, result: 400Hz - 1500Hz.
    unsigned char sidetone_freq_50hz;

    // 5 - 50 wpm.
    unsigned char speed_wpm;

    // result = v * 50
    // v: 0 - 40, result: 0ms - 2000ms.
    unsigned char tx_delay_50ms;

    // 7  6  5  4  3  2  1  0
    //             |  |  |  |                  0                 1
    //             |  |  |  +-- Disable Sidetone | Enable Sidetone
    //             |  |  +-----       Disable TX |       Enable TX
    //             |  +--------       Right-Hand |       Left-Hand
    //             +-----------        Iambic-B |        Iamboic-A
    signed char flags;

    char callsign[CFG_CALLSIGN_BUFFER_LEN];
} config;

extern config _config;

#define CFG_FLAG_SIDETONE_SHIFT 0
#define CFG_FLAG_TX_SHIFT 1
#define CFG_FLAG_HAND_SHIFT 2
#define CFG_FLAG_IAMBIC_MODE_SHIFT 3

#define CFG_FLAG_SIDETONE_ENABLED (_config.flags & (1 << CFG_FLAG_SIDETONE_SHIFT))
#define CFG_FLAG_ENABLE_SIDETONE (_conig.flags |= (1 << CFG_FLAG_SIDETONE_SHIFT))
#define CFG_FLAG_DISABLE_SIDETONE (_conig.flags &= ~(1 << CFG_FLAG_SIDETONE_SHIFT))

#define CFG_FLAG_TX_ENABLED (_config.flags & (1 << CFG_FLAG_TX_SHIFT))
#define CFG_FLAG_ENABLE_TX (_config.flags |= (1 << CFG_FLAG_TX_SHIFT))
#define CFG_FLAG_DISABLE_TX (_config.flags &= ~(1 << CFG_FLAG_TX_SHIFT))

#define CFG_FLAG_IS_LEFT_HAND (_config.flags & (1 << CFG_FLAG_HAND_SHIFT))
#define CFG_FLAG_SET_LEFT_HAND (_config.flags |= (1 << CFG_FLAG_HAND_SHIFT))
#define CFG_FLAG_SET_RIGHT_HAND (_config.flags &= ~(1 << CFG_FLAG_HAND_SHIFT))

#define CFG_FLAG_IS_IAMBIC_A (_config.flags & (1 << CFG_FLAG_IAMBIC_MODE_SHIFT))
#define CFG_FLAG_SET_IAMBIC_A (_config.flags |= (1 << CFG_FLAG_IAMBIC_MODE_SHIFT))
#define CFG_FLAG_SET_IAMBIC_B (_config.flags &= ~(1 << CFG_FLAG_IAMBIC_MODE_SHIFT))

#define CFG_FLAG_SET_DEFAULTS (_config.flags = (1 << CFG_FLAG_SIDETONE_SHIFT) | (1 << CFG_FLAG_TX_SHIFT))

// CW element duration in ms.
// (1200 / 50) - (1200 / 5)
// 24ms - 240ms
extern unsigned char _runtime_cw_ems;

extern const char *_runtime_autotext_ptr;
extern char _runtime_resp_buffer[];

// 7  6  5  4  3  2  1  0
// |  |  |  |  |  |  |  |                0               1
// |  |  |  |  |  |  |  +--    Sidetone OFF |   Sidetone ON
// |  |  |  |  |  |  +-----     TX Disabled |   TX Enabled
// |  |  |  |  |  +--------          Normal |  Setting Mode
// |  |  |  |  +------------         Paddle |  Straight Key
// |  |  |  +--------------          Normal |   Interactive
// |  |  +-----------------          Normal |       Auto CQ
// |  +-------------------- Paddle Disabled | Paddle Enabled
// +--------------------------- SW Disabled |     SW Enabled
extern signed char _runtime_flags;

#define RT_FLAG_SIDETONE_SHIFT 0
#define RT_FLAG_TX_SHIFT 1
#define RT_FLAG_SETTING_MODE_SHIFT 2
#define RT_FLAG_STRAIGHT_KEY_SHIFT 3
#define RT_FLAG_INTERACTIVE_SHIFT 4
#define RT_FLAG_AUTOCQ_SHIFT 5
#define RT_FLAG_PADDLE_SHIFT 6
#define RT_FLAG_SW_SHIFT 7

#define RT_FLAG_IS_SIDETONE_ON (_runtime_flags & (1 << RT_FLAG_SIDETONE_SHIFT))
#define RT_FLAG_SET_SIDETONE_ON (_runtime_flags |= (1 << RT_FLAG_SIDETONE_SHIFT))
#define RT_FLAG_SET_SIDETONE_OFF (_runtime_flags &= ~(1 << RT_FLAG_SIDETONE_SHIFT))

#define RT_FLAG_TX_ENABLED (_runtime_flags & (1 << RT_FLAG_TX_SHIFT))
#define RT_FLAG_ENABLE_TX (_runtime_flags |= (1 << RT_FLAG_TX_SHIFT))
#define RT_FLAG_DISABLE_TX (_runtime_flags &= ~(1 << RT_FLAG_TX_SHIFT))

#define RT_FLAG_SETTING_MODE_ENABLED (_runtime_flags & (1 << RT_FLAG_SETTING_MODE_SHIFT))
#define RT_FLAG_ENABLE_SETTING_MODE (_runtime_flags |= (1 << RT_FLAG_SETTING_MODE_SHIFT))
#define RT_FLAG_DISABLE_SETTING_MODE (_runtime_flags &= ~(1 << RT_FLAG_SETTING_MODE_SHIFT))

#define RT_FLAG_STRAIGHT_KEY_ENABLED (_runtime_flags & (1 << RT_FLAG_STRAIGHT_KEY_SHIFT))
#define RT_FLAG_ENABLE_STRAIGHT_KEY (_runtime_flags |= (1 << RT_FLAG_STRAIGHT_KEY_SHIFT))
#define RT_FLAG_DISABLE_STRAIGHT_KEY (_runtime_flags &= ~(1 << RT_FLAG_STRAIGHT_KEY_SHIFT))

#define RT_FLAG_INTERACTIVE_ENABLED (_runtime_flags & (1 << RT_FLAG_INTERACTIVE_SHIFT))
#define RT_FLAG_ENABLE_INTERACTIVE (_runtime_flags |= (1 << RT_FLAG_INTERACTIVE_SHIFT))
#define RT_FLAG_DISABLE_INTERACTIVE (_runtime_flags &= ~(1 << RT_FLAG_INTERACTIVE_SHIFT))

#define RT_FLAG_AUTOCQ_ENABLED (_runtime_flags & (1 << RT_FLAG_AUTOCQ_SHIFT))
#define RT_FLAG_ENABLE_AUTOCQ (_runtime_flags |= (1 << RT_FLAG_AUTOCQ_SHIFT))
#define RT_FLAG_DISABLE_AUTOCQ (_runtime_flags &= ~(1 << RT_FLAG_AUTOCQ_SHIFT))

#define RT_FLAG_PADDLE_ENABLED (_runtime_flags & (1 << RT_FLAG_PADDLE_SHIFT))
#define RT_FLAG_ENABLE_PADDLE (_runtime_flags |= (1 << RT_FLAG_PADDLE_SHIFT))
#define RT_FLAG_DISABLE_PADDLE (_runtime_flags &= ~(1 << RT_FLAG_PADDLE_SHIFT))

#define RT_FLAG_SW_ENABLED (_runtime_flags & (1 << RT_FLAG_SW_SHIFT))
#define RT_FLAG_ENABLE_SW (_runtime_flags |= (1 << RT_FLAG_SW_SHIFT))
#define RT_FLAG_DISABLE_SW (_runtime_flags &= ~(1 << RT_FLAG_SW_SHIFT))

#define RT_FLAG_SET_DEFAULTS (_runtime_flags = (1 << RT_FLAG_TX_SHIFT) | (1 << RT_FLAG_PADDLE_SHIFT) | (1 << RT_FLAG_SW_SHIFT))


void reset_config_and_runtime_values();
void load_config();
void save_config();

void cfg_reset();
void cfg_set_speed_wpm(unsigned char value);
void cfg_set_sidetone_freq_50hz(unsigned char value);
void cfg_set_tx_delay_50ms(unsigned char value);

unsigned int times_50(unsigned char v);
unsigned int divide(unsigned int a, unsigned int b);

// decoded buffer
void rt_morse_decoded_buffer_reset();
char rt_morse_decoded_buffer_getch();
void rt_morse_decoded_buffer_putch(char ch);

#endif // __CONFIG_H__
