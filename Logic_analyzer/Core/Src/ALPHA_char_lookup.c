/*
 * DOGS_character_lookup.c
 *
 *  Created on: Oct 22, 2025
 *      Author: trloeffler20
 */

#include "main.h"

// every character sent has an ascii value
// we can use that ascii value as an index in this table to generate the correct hex value for the screen


const uint8_t table [123] = {
		[0] = 0x30, // 0
		[1] = 0x31, // 1
		[2] = 0x32, // 2
		[3] = 0x33, // 3
		[4] = 0x34, // 4
		[5] = 0x35, // 5
		[6] = 0x36, // 6
		[7] = 0x37, // 7
		[8] = 0x38, // 8
		[9] = 0x39, // 9
		[10] = 0x41, // 10/'A'
	    [11] = 0x42, // 11/'B'
	    [12] = 0x43, // 12/'C'
	    [13] = 0x44, // 13/'D'
	    [14] = 0x45, // 14/'E'
	    [15] = 0x46, // 15/'F'
		[32] = 0x20, // ' '
		[58] = 0x3A, // ':'
	    [63] = 0x3F, // '?'
		[65] = 0x41, // 'A'
		[66] = 0x42, // 'B'
	    [67] = 0x43, // 'C'
	    [68] = 0x44, // 'D'
	    [69] = 0x45, // 'E'
	    [70] = 0x46, // 'F'
	    [71] = 0x47, // 'G'
	    [72] = 0x48, // 'H'
	    [73] = 0x49, // 'I'
	    [74] = 0x4A, // 'J'
	    [75] = 0x4B, // 'K'
	    [76] = 0x4C, // 'L'
	    [77] = 0x4D, // 'M'
	    [78] = 0x4E, // 'N'
	    [79] = 0x4F, // 'O'
	    [80] = 0x50, // 'P'
	    [81] = 0x51, // 'Q'
	    [82] = 0x52, // 'R'
	    [83] = 0x53, // 'S'
	    [84] = 0x54, // 'T'
	    [85] = 0x55, // 'U'
	    [86] = 0x56, // 'V'
	    [87] = 0x57, // 'W'
	    [88] = 0x58, // 'X'
	    [89] = 0x59, // 'Y'
	    [90] = 0x5A, // 'Z'
	    [97] = 0x61, // 'a'
	    [98] = 0x62, // 'b'
	    [99] = 0x63, // 'c'
	    [100] = 0x64, // 'd'
	    [101] = 0x65, // 'e'
	    [102] = 0x66, // 'f'
	    [103] = 0x67, // 'g'
	    [104] = 0x68, // 'h'
	    [105] = 0x69, // 'i'
	    [106] = 0x6A, // 'j'
	    [107] = 0x6B, // 'k'
	    [108] = 0x6C, // 'l'
	    [109] = 0x6D, // 'm'
	    [110] = 0x6E, // 'n'
	    [111] = 0x6F, // 'o'
	    [112] = 0x70, // 'p'
	    [113] = 0x71, // 'q'
	    [114] = 0x72, // 'r'
	    [115] = 0x73, // 's'
	    [116] = 0x74, // 't'
	    [117] = 0x75, // 'u'
	    [118] = 0x76, // 'v'
	    [119] = 0x77, // 'w'
	    [120] = 0x78, // 'x'
	    [121] = 0x79, // 'y'
	    [122] = 0x7A  // 'z'
};




int8_t char_to_DOGShex(int8_t data){

	uint8_t offset = (uint8_t)data; // ' '(space) = 0x20, the lowest ascii value we will use
									// every other character will have a larger ascii value
									// and create an offset in this variable init calculation
	return table[offset];
}





