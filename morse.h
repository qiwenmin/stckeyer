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

#ifndef __MORSE_H__
#define __MORSE_H__

#define MORSE_PATTERN_FINISHED (0b10000000)

#if defined(COMPACT_CODE)

#define morse_get_pattern(v) (v)
#define morse_get_char(v) (v)

#else // !COMPACT_CODE

unsigned char morse_get_pattern(char ch);
char morse_get_char(unsigned char pattern);

#endif // COMPACT_CODE

#endif // __MORSE_H__