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

#ifndef __HW_H__
#define __HW_H__

#include "tnyfsmos/stc15.h"

#define SW_PIN P3_4
#define DIT_PIN P3_0
#define DAH_PIN P3_1

#define INPUTS_PORT P3

// 7  6  5  4  3  2  1  0
//          |        |  +-- DIT PIN
//          |        +----- DAH PIN
//          +--------------  SW PIN

#define HW_SW_MASK (1 << 4)
#define HW_DAH_MASK (1 << 1)
#define HW_DIT_MASK (1 << 0)
#define HW_INPUTS_MASK (HW_SW_MASK | HW_DAH_MASK | HW_DIT_MASK)

#define HW_ANY_INPUT_DOWN (~(INPUTS_PORT & INPUTS_MASK))

#endif // __HW_H__
