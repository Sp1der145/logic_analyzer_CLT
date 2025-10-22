/*
 * DOGS_character_lookup.c
 *
 *  Created on: Oct 22, 2025
 *      Author: trloeffler20
 */

#include <stdint.h>

uint8_t char_to_DOGShex(uint8_t data){
	uint8_t hex;
	switch (data){
		case ' ':
			hex = 0x20;
			break;
		case '?':
			hex = 0x3F;
			break;
		case 'A':
		case 10:
	        hex = 0x41;
	        break;
	    case 'B':
	    case 11:
	        hex = 0x42;
	        break;
	    case 'C':
	    case 12:
	        hex = 0x43;
	        break;
	    case 'D':
	    case 13:
	        hex = 0x44;
	        break;
	    case 'E':
	    case 14:
	        hex = 0x45;
	        break;
	    case 'F':
	    case 15:
	        hex = 0x46;
	        break;
	    case 'G':
	        hex = 0x47;
	        break;
	    case 'H':
	        hex = 0x48;
	        break;
	    case 'I':
	        hex = 0x49;
	        break;
	    case 'J':
	        hex = 0x4A;
	        break;
	    case 'K':
	        hex = 0x4B;
	        break;
	    case 'L':
	        hex = 0x4C;
	        break;
	    case 'M':
	        hex = 0x4D;
	        break;
	    case 'N':
	        hex = 0x4E;
	        break;
	    case 'O':
	        hex = 0x4F;
	        break;
	    case 'P':
	        hex = 0x50;
	        break;
	    case 'Q':
	        hex = 0x51;
	        break;
	    case 'R':
	        hex = 0x52;
	        break;
	    case 'S':
	        hex = 0x53;
	        break;
	    case 'T':
	        hex = 0x54;
	        break;
	    case 'U':
	        hex = 0x55;
	        break;
	    case 'V':
	        hex = 0x56;
	        break;
	    case 'W':
	        hex = 0x57;
	        break;
	    case 'X':
	        hex = 0x58;
	        break;
	    case 'Y':
	        hex = 0x59;
	        break;
	    case 'Z':
	        hex = 0x5A;
	        break;
	    case 'a':
			hex = 0x60;
			break;
		case 'b':
			hex = 0x61;
			break;
		case 'c':
			hex = 0x62;
			break;
		case 'd':
			hex = 0x63;
			break;
		case 'e':
			hex = 0x64;
			break;
		case 'f':
			hex = 0x65;
			break;
		case 'g':
			hex = 0x66;
			break;
		case 'h':
			hex = 0x67;
			break;
		case 'i':
			hex = 0x68;
			break;
		case 'j':
			hex = 0x69;
			break;
		case 'k':
			hex = 0x6A;
			break;
		case 'l':
			hex = 0x6B;
			break;
		case 'm':
			hex = 0x6C;
			break;
		case 'n':
			hex = 0x6D;
			break;
		case 'o':
			hex = 0x6E;
			break;
		case 'p':
			hex = 0x70;
			break;
		case 'q':
			hex = 0x71;
			break;
		case 'r':
			hex = 0x72;
			break;
		case 's':
			hex = 0x73;
			break;
		case 't':
			hex = 0x74;
			break;
		case 'u':
			hex = 0x75;
			break;
		case 'v':
			hex = 0x76;
			break;
		case 'w':
			hex = 0x77;
			break;
		case 'x':
			hex = 0x78;
			break;
		case 'y':
			hex = 0x79;
			break;
		case 'z':
			hex = 0x7A;
			break;
		case 0:
			hex = 0x30;
			break;
		case 1:
			hex = 0x31;
			break;
		case 2:
			hex = 0x32;
			break;
		case 3:
			hex = 0x33;
			break;
		case 4:
			hex = 0x34;
			break;
		case 5:
			hex = 0x35;
			break;
		case 6:
			hex = 0x36;
			break;
		case 7:
			hex = 0x37;
			break;
		case 8:
			hex = 0x38;
			break;
		case 9:
			hex = 0x39;
			break;
		case ':':
			hex = 0x3A;
			break;
		default:
			hex = 0xDB; // Box character to know something went wrong
			break;

	}
	return hex;
}
