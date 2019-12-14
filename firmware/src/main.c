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
#include "sleep.h"

TFO_INIT(TASK_COUNT)

void timer0_isr() __interrupt TF0_VECTOR;
void int4_isr() __interrupt 16 {} // wake on int4

void main() {
    tfo_init_os();

    reset_config_and_runtime_values();
    load_config();

    sleep_init();

    while (1) {
        sw_state_machine();
        paddle_state_machine();
        setting_state_machine();
        autotext_state_machine();
        keying_state_machine();
        txing_state_machine();

        sleep_check();
    }
}
