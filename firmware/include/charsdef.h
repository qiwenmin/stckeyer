#ifndef __CHARSDEF_H__
#define __CHARSDEF_H__

#if defined(COMPACT_CODE)

#define MCH_SP 0b10000000
#define MCH_LF 0b11111111
#define MCH_EM 0b10101110
#define MCH_QM 0b00110010
#define MCH_SLASH 0b10010100

#define MCH_0 0b11111100
#define MCH_1 0b01111100
#define MCH_2 0b00111100
#define MCH_3 0b00011100
#define MCH_4 0b00001100
#define MCH_5 0b00000100
#define MCH_6 0b10000100
#define MCH_7 0b11000100
#define MCH_8 0b11100100
#define MCH_9 0b11110100

#define MCH_A 0b01100000
#define MCH_B 0b10001000
#define MCH_C 0b10101000
#define MCH_D 0b10010000
#define MCH_E 0b01000000
#define MCH_F 0b00101000
#define MCH_K 0b10110000
#define MCH_L 0b01001000
#define MCH_M 0b11100000
#define MCH_N 0b10100000
#define MCH_O 0b11110000
#define MCH_P 0b01101000
#define MCH_Q 0b11011000
#define MCH_R 0b01010000
#define MCH_S 0b00010000
#define MCH_T 0b11000000
#define MCH_V 0b00011000
#define MCH_W 0b01110000
#define MCH_X 0b10011000
#define MCH_Y 0b10111000

extern __code char __num_pattern_table[];

#define NUM2CH(v) __num_pattern_table[(v)]

#else // !COMPACT_CODE

#define MCH_SP ' '
#define MCH_LF '\n'
#define MCH_EM '!'
#define MCH_QM '?'
#define MCH_SLASH '/'

#define MCH_0 '0'
#define MCH_1 '1'
#define MCH_2 '2'
#define MCH_3 '3'
#define MCH_4 '4'
#define MCH_5 '5'
#define MCH_6 '6'
#define MCH_7 '7'
#define MCH_8 '8'
#define MCH_9 '9'

#define MCH_A 'A'
#define MCH_B 'B'
#define MCH_C 'C'
#define MCH_D 'D'
#define MCH_E 'E'
#define MCH_F 'F'
#define MCH_K 'K'
#define MCH_L 'L'
#define MCH_M 'M'
#define MCH_N 'N'
#define MCH_O 'O'
#define MCH_P 'P'
#define MCH_Q 'Q'
#define MCH_R 'R'
#define MCH_S 'S'
#define MCH_T 'T'
#define MCH_V 'V'
#define MCH_W 'W'
#define MCH_X 'X'
#define MCH_Y 'Y'

#define NUM2CH(v) ((v) + '0')

#endif // COMPACT_CODE

#endif // __CHARSDEF_H__
