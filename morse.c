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

#include "morse.h"

static __code const unsigned char morse_table[] = {
    0b10101110, // ! _._.__
    0b01001010, // " ._.._.
    0b00010110, // # ..._._ SK
    0b00010011, // $ ..._.._
    0b10000000, // % ??????
    0b01000100, // & ._...
    0b01111010, // ' .____.
    0b10110100, // ( _.__.
    0b10110110, // ) _.__._
    0b10000000, // * ??????
    0b01010100, // + ._._.
    0b11001110, // , __..__
    0b10000110, // - _...._
    0b01010110, // . ._._._
    0b10010100, // / _.._.
    0b11111100, // 0 _____
    0b01111100, // 1 .____
    0b00111100, // 2 ..___
    0b00011100, // 3 ...__
    0b00001100, // 4 ...._
    0b00000100, // 5 .....
    0b10000100, // 6 _....
    0b11000100, // 7 __...
    0b11100100, // 8 ___..
    0b11110100, // 9 ____.
    0b11100010, // : ___...
    0b10101010, // ; _._._.
    0b10000000, // < ??????
    0b10001100, // = _..._
    0b10000000, // > ??????
    0b00110010, // ? ..__..
    0b01101010, // @ .__._.
    0b01100000, // A ._
    0b10001000, // B _...
    0b10101000, // C _._.
    0b10010000, // D _..
    0b01000000, // E .
    0b00101000, // F .._.
    0b11010000, // G __.
    0b00001000, // H ....
    0b00100000, // I ..
    0b01111000, // J .___
    0b10110000, // K _._
    0b01001000, // L ._..
    0b11100000, // M __
    0b10100000, // N _.
    0b11110000, // O ___
    0b01101000, // P .__.
    0b11011000, // Q __._
    0b01010000, // R ._.
    0b00010000, // S ...
    0b11000000, // T _
    0b00110000, // U .._
    0b00011000, // V ..._
    0b01110000, // W .__
    0b10011000, // X .__.
    0b10111000, // Y _.__
    0b11001000, // Z __..

    0b00110110 // _ ..__._
};

static const unsigned char under_score_index = sizeof(morse_table) - 1;

unsigned char morse_get_pattern(char ch) {
    char idx = 0xFF;

    // to uppercase
    if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';

    if (ch >= '!' && ch <= 'Z') {
        idx = ch - '!';
    } else if (ch == '_') {
        idx = under_score_index;
    }

    if (idx != 0xFF) {
        return morse_table[idx];
    } else {
        return MORSE_PATTERN_FINISHED;
    }
}

char morse_get_char(unsigned char pattern) {
    unsigned char idx;
    for (idx = 0; idx < sizeof(morse_table); idx ++) {
        if (pattern == morse_table[idx]) {
            if (idx == under_score_index) {
                return '_';
            } else {
                return idx + '!';
            }
        }
    }

    return 0;
}