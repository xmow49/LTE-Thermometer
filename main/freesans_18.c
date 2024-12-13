/*******************************************************************************
 * Size: 18 px
 * Bpp: 1
 * Opts: --bpp 1 --size 18 --no-compress --font FreeSansBold.ttf --symbols °éèê% --range 32-127 --format lvgl -o freesans_18.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef FREESANS_18
#define FREESANS_18 1
#endif

#if FREESANS_18

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xfa, 0x40, 0x7e,

    /* U+0022 "\"" */
    0xef, 0xdf, 0xba, 0x20,

    /* U+0023 "#" */
    0x19, 0x86, 0x41, 0x91, 0xff, 0x7f, 0xc4, 0xc3,
    0x30, 0xc8, 0xff, 0xbf, 0xe2, 0x60, 0x98, 0x66,
    0x19, 0x0,

    /* U+0024 "$" */
    0x8, 0x1f, 0x1f, 0xd9, 0x7c, 0xbe, 0x41, 0xe0,
    0xfc, 0xf, 0x5, 0xf2, 0xf9, 0x77, 0xf1, 0xf0,
    0x20, 0x10,

    /* U+0025 "%" */
    0x38, 0x20, 0xf8, 0xc3, 0x19, 0x6, 0x36, 0xc,
    0x68, 0xf, 0xb0, 0xe, 0x4e, 0x1, 0xbe, 0x2,
    0xc6, 0xd, 0x8c, 0x13, 0x18, 0x63, 0xe0, 0x83,
    0x80,

    /* U+0026 "&" */
    0x1e, 0xf, 0xe1, 0xcc, 0x39, 0x83, 0xe0, 0x38,
    0xf, 0xb7, 0xb6, 0xe3, 0xdc, 0x73, 0xce, 0x3f,
    0xe3, 0xee,

    /* U+0027 "'" */
    0xff, 0xa0,

    /* U+0028 "(" */
    0x31, 0x98, 0xc6, 0x63, 0x18, 0xc6, 0x31, 0x86,
    0x31, 0x86, 0x30,

    /* U+0029 ")" */
    0x63, 0xc, 0x63, 0xc, 0x63, 0x18, 0xc6, 0x33,
    0x18, 0xcc, 0x60,

    /* U+002A "*" */
    0x21, 0x3e, 0xe5, 0x28,

    /* U+002B "+" */
    0x18, 0xc, 0x6, 0x3, 0xf, 0xff, 0xfc, 0x60,
    0x30, 0x18, 0x0,

    /* U+002C "," */
    0xfc, 0xbc,

    /* U+002D "-" */
    0xff, 0xc0,

    /* U+002E "." */
    0xfc,

    /* U+002F "/" */
    0x8, 0xc4, 0x23, 0x10, 0x84, 0x62, 0x11, 0x88,
    0x0,

    /* U+0030 "0" */
    0x3e, 0x3f, 0x9d, 0xdc, 0x7e, 0x3f, 0x1f, 0x8f,
    0xc7, 0xe3, 0xf1, 0xdd, 0xcf, 0xe3, 0xe0,

    /* U+0031 "1" */
    0xc, 0x7f, 0xff, 0x1c, 0x71, 0xc7, 0x1c, 0x71,
    0xc7, 0x1c,

    /* U+0032 "2" */
    0x3e, 0x3f, 0xbc, 0xfc, 0x7e, 0x38, 0x1c, 0x1c,
    0x1c, 0x3c, 0x3c, 0x3c, 0x1f, 0xff, 0xf8,

    /* U+0033 "3" */
    0x3c, 0x3f, 0xb9, 0xdc, 0xe0, 0x70, 0x70, 0x3c,
    0xf, 0x3, 0xf1, 0xf9, 0xef, 0xe3, 0xe0,

    /* U+0034 "4" */
    0xf, 0x7, 0x87, 0xc2, 0xe3, 0x73, 0x39, 0x9d,
    0x8e, 0xff, 0xff, 0xc1, 0xc0, 0xe0, 0x70,

    /* U+0035 "5" */
    0x3f, 0x3f, 0x98, 0xc, 0x7, 0xe3, 0xfb, 0x9e,
    0x7, 0x3, 0xf1, 0xf9, 0xef, 0xe3, 0xe0,

    /* U+0036 "6" */
    0x1e, 0x3f, 0x9c, 0xfc, 0xe, 0xe7, 0xfb, 0xdf,
    0xc7, 0xe3, 0xf1, 0xdd, 0xef, 0xe3, 0xe0,

    /* U+0037 "7" */
    0xff, 0xff, 0xc0, 0xc0, 0xe0, 0x60, 0x60, 0x30,
    0x38, 0x18, 0x1c, 0xe, 0x7, 0x3, 0x80,

    /* U+0038 "8" */
    0x1e, 0xf, 0xc7, 0x39, 0x86, 0x71, 0x8f, 0xc7,
    0xfb, 0xcf, 0xe1, 0xf8, 0x7f, 0x3d, 0xfe, 0x3f,
    0x0,

    /* U+0039 "9" */
    0x3e, 0x3f, 0xb9, 0xd8, 0x7c, 0x3f, 0x3d, 0xfe,
    0x77, 0x3, 0xe1, 0xf9, 0xcf, 0xe3, 0xc0,

    /* U+003A ":" */
    0xfc, 0x0, 0x0, 0xfc,

    /* U+003B ";" */
    0xfc, 0x0, 0x0, 0xfc, 0xbc,

    /* U+003C "<" */
    0x0, 0x1, 0xc7, 0xff, 0x8e, 0x7, 0x80, 0xf8,
    0x1f, 0x1, 0x80,

    /* U+003D "=" */
    0xff, 0xff, 0xc0, 0x0, 0xf, 0xff, 0xfc,

    /* U+003E ">" */
    0x0, 0x70, 0x3f, 0x3, 0xf0, 0x38, 0x3c, 0xf9,
    0xf0, 0xc0, 0x0,

    /* U+003F "?" */
    0x3e, 0x3f, 0xbd, 0xfc, 0x7e, 0x38, 0x1c, 0x1c,
    0x1c, 0x18, 0xc, 0x0, 0x3, 0x81, 0xc0,

    /* U+0040 "@" */
    0x3, 0xf0, 0x7, 0xfe, 0x7, 0x3, 0x86, 0x3b,
    0xe6, 0x3f, 0x3b, 0x31, 0x8f, 0x38, 0xc7, 0x98,
    0x63, 0xcc, 0x23, 0xe6, 0x33, 0xb3, 0xff, 0xcc,
    0xf3, 0x87, 0x0, 0x1, 0xe0, 0x80, 0x7f, 0xc0,
    0xf, 0xc0,

    /* U+0041 "A" */
    0x7, 0x0, 0x3c, 0x3, 0xe0, 0x1b, 0x1, 0xdc,
    0xe, 0xe0, 0x63, 0x7, 0x1c, 0x3f, 0xe1, 0xff,
    0x1c, 0x1c, 0xe0, 0xe6, 0x3, 0x0,

    /* U+0042 "B" */
    0xff, 0x1f, 0xf3, 0x87, 0x70, 0xee, 0x1d, 0xff,
    0x3f, 0xe7, 0xe, 0xe0, 0xfc, 0x1f, 0x83, 0xff,
    0xef, 0xf8,

    /* U+0043 "C" */
    0x1f, 0x83, 0xfc, 0x78, 0xef, 0x6, 0xe0, 0xe,
    0x0, 0xe0, 0xe, 0x0, 0xe0, 0x7f, 0x6, 0x78,
    0xe3, 0xfc, 0x1f, 0x80,

    /* U+0044 "D" */
    0xff, 0x1f, 0xf3, 0x87, 0x70, 0xfe, 0xf, 0xc1,
    0xf8, 0x3f, 0x7, 0xe0, 0xfc, 0x3f, 0x87, 0x7f,
    0xcf, 0xf0,

    /* U+0045 "E" */
    0xff, 0xff, 0xfe, 0x3, 0x80, 0xe0, 0x3f, 0xef,
    0xfb, 0x80, 0xe0, 0x38, 0xe, 0x3, 0xff, 0xff,
    0xc0,

    /* U+0046 "F" */
    0xff, 0xff, 0xf8, 0x1c, 0xe, 0x7, 0xff, 0xff,
    0xc0, 0xe0, 0x70, 0x38, 0x1c, 0xe, 0x0,

    /* U+0047 "G" */
    0xf, 0x83, 0xfe, 0x78, 0xe7, 0x7, 0xe0, 0xe,
    0x0, 0xe1, 0xfe, 0x1f, 0xe0, 0x37, 0x7, 0x78,
    0xf3, 0xff, 0xf, 0xb0,

    /* U+0048 "H" */
    0xe0, 0xfc, 0x1f, 0x83, 0xf0, 0x7e, 0xf, 0xff,
    0xff, 0xff, 0x7, 0xe0, 0xfc, 0x1f, 0x83, 0xf0,
    0x7e, 0xe,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xff, 0xfe,

    /* U+004A "J" */
    0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
    0xe7, 0xe7, 0xe7, 0x7e, 0x3c,

    /* U+004B "K" */
    0xe0, 0xee, 0x1c, 0xe3, 0x8e, 0x70, 0xee, 0xf,
    0xc0, 0xfe, 0xf, 0x70, 0xe7, 0xe, 0x38, 0xe1,
    0xce, 0xe, 0xe0, 0xf0,

    /* U+004C "L" */
    0xe0, 0x70, 0x38, 0x1c, 0xe, 0x7, 0x3, 0x81,
    0xc0, 0xe0, 0x70, 0x38, 0x1f, 0xff, 0xf8,

    /* U+004D "M" */
    0xf0, 0x7f, 0xc7, 0xfe, 0x3f, 0xf1, 0xff, 0x8f,
    0xf4, 0x5f, 0xb6, 0xfd, 0xb7, 0xed, 0xbf, 0x6d,
    0xf9, 0xcf, 0xce, 0x7e, 0x73, 0x80,

    /* U+004E "N" */
    0xe0, 0xfe, 0x1f, 0xc3, 0xfc, 0x7f, 0xcf, 0xd9,
    0xfb, 0xbf, 0x37, 0xe7, 0xfc, 0x7f, 0x87, 0xf0,
    0xfe, 0xe,

    /* U+004F "O" */
    0xf, 0x81, 0xff, 0x1e, 0x3c, 0xe0, 0xee, 0x3,
    0xf0, 0x1f, 0x80, 0xfc, 0x7, 0xe0, 0x3b, 0x83,
    0x9e, 0x3c, 0x7f, 0xc0, 0xf8, 0x0,

    /* U+0050 "P" */
    0xff, 0x3f, 0xee, 0x1f, 0x87, 0xe1, 0xf8, 0x7f,
    0xfb, 0xfc, 0xe0, 0x38, 0xe, 0x3, 0x80, 0xe0,
    0x0,

    /* U+0051 "Q" */
    0xf, 0x81, 0xff, 0x1e, 0x3c, 0xe0, 0xee, 0x3,
    0xf0, 0x1f, 0x80, 0xfc, 0x7, 0xe1, 0xbb, 0x8f,
    0x9e, 0x3c, 0x7f, 0xe0, 0xfb, 0x80, 0x8,

    /* U+0052 "R" */
    0xff, 0x9f, 0xfb, 0x83, 0xf0, 0x7e, 0xf, 0xc1,
    0xbf, 0xe7, 0xfc, 0xe1, 0xdc, 0x3b, 0x87, 0x70,
    0xee, 0x1e,

    /* U+0053 "S" */
    0x3f, 0xf, 0xf3, 0x8f, 0x70, 0xee, 0x1, 0xfc,
    0x1f, 0xf0, 0x3f, 0x0, 0xfc, 0x1f, 0xc7, 0xbf,
    0xe3, 0xf8,

    /* U+0054 "T" */
    0xff, 0xff, 0xfc, 0x38, 0x7, 0x0, 0xe0, 0x1c,
    0x3, 0x80, 0x70, 0xe, 0x1, 0xc0, 0x38, 0x7,
    0x0, 0xe0,

    /* U+0055 "U" */
    0xe0, 0xfc, 0x1f, 0x83, 0xf0, 0x7e, 0xf, 0xc1,
    0xf8, 0x3f, 0x7, 0xe0, 0xfc, 0x1f, 0xc7, 0xbf,
    0xe1, 0xf0,

    /* U+0056 "V" */
    0x60, 0x67, 0xe, 0x70, 0xe3, 0xc, 0x30, 0xc3,
    0x9c, 0x19, 0x81, 0x98, 0x1f, 0x80, 0xf0, 0xf,
    0x0, 0xf0, 0x6, 0x0,

    /* U+0057 "W" */
    0xe1, 0xc3, 0xb0, 0xe1, 0x9c, 0x71, 0xce, 0x6c,
    0xe3, 0x36, 0x71, 0x9b, 0x30, 0xcd, 0x98, 0x76,
    0x5c, 0x1e, 0x3c, 0xf, 0x1e, 0x7, 0x8f, 0x1,
    0xc7, 0x80, 0xc1, 0x80,

    /* U+0058 "X" */
    0x70, 0xe7, 0x8e, 0x39, 0xc1, 0xf8, 0x1f, 0x80,
    0xf0, 0xf, 0x0, 0xf0, 0x1f, 0x81, 0x9c, 0x39,
    0xc7, 0xe, 0x70, 0xe0,

    /* U+0059 "Y" */
    0xe0, 0xfc, 0x39, 0xc7, 0x39, 0xc3, 0xb8, 0x36,
    0x7, 0xc0, 0x70, 0xe, 0x1, 0xc0, 0x38, 0x7,
    0x0, 0xe0,

    /* U+005A "Z" */
    0xff, 0xff, 0xf0, 0x3c, 0xe, 0x7, 0x3, 0x81,
    0xe0, 0x70, 0x38, 0x1c, 0xf, 0x3, 0xff, 0xff,
    0xc0,

    /* U+005B "[" */
    0xff, 0xf9, 0xce, 0x73, 0x9c, 0xe7, 0x39, 0xce,
    0x73, 0x9f, 0xf8,

    /* U+005C "\\" */
    0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x10,
    0x80,

    /* U+005D "]" */
    0xff, 0xce, 0x73, 0x9c, 0xe7, 0x39, 0xce, 0x73,
    0x9c, 0xff, 0xf8,

    /* U+005E "^" */
    0x1c, 0x3c, 0x3c, 0x36, 0x66, 0x66, 0x63, 0xc3,

    /* U+005F "_" */
    0xff, 0xff, 0xfc,

    /* U+0060 "`" */
    0xe6, 0x30,

    /* U+0061 "a" */
    0x3e, 0x1f, 0xee, 0x38, 0xe, 0x3f, 0x9e, 0xee,
    0x3b, 0x9e, 0xff, 0x9e, 0xe0,

    /* U+0062 "b" */
    0xe0, 0x38, 0xe, 0x3, 0xbc, 0xff, 0xbc, 0xee,
    0x1f, 0x87, 0xe1, 0xf8, 0x7f, 0x3b, 0xfe, 0xef,
    0x0,

    /* U+0063 "c" */
    0x3e, 0x3f, 0xbc, 0xdc, 0x6e, 0x7, 0x3, 0x8f,
    0xe6, 0x7f, 0x1f, 0x0,

    /* U+0064 "d" */
    0x1, 0xc0, 0x70, 0x1c, 0xf7, 0x7f, 0xdc, 0xfe,
    0x1f, 0x87, 0xe1, 0xf8, 0x7f, 0x3d, 0xff, 0x3d,
    0xc0,

    /* U+0065 "e" */
    0x3e, 0x3f, 0xb8, 0xdc, 0x7f, 0xff, 0xff, 0x81,
    0xc7, 0x7f, 0x1f, 0x0,

    /* U+0066 "f" */
    0x3b, 0xdd, 0xff, 0xb9, 0xce, 0x73, 0x9c, 0xe7,
    0x0,

    /* U+0067 "g" */
    0x3b, 0xbf, 0xfd, 0xfc, 0x7e, 0x3f, 0x1f, 0x8f,
    0xef, 0x7f, 0x9d, 0xc0, 0xfc, 0x77, 0xf1, 0xf0,

    /* U+0068 "h" */
    0xe0, 0x70, 0x38, 0x1d, 0xef, 0xff, 0x9f, 0x8f,
    0xc7, 0xe3, 0xf1, 0xf8, 0xfc, 0x7e, 0x38,

    /* U+0069 "i" */
    0xfc, 0x7f, 0xff, 0xff, 0xfe,

    /* U+006A "j" */
    0x77, 0x7, 0x77, 0x77, 0x77, 0x77, 0x77, 0x7f,
    0xe0,

    /* U+006B "k" */
    0xe0, 0x70, 0x38, 0x1c, 0x7e, 0x77, 0x73, 0xf1,
    0xf8, 0xfe, 0x77, 0x39, 0xdc, 0x6e, 0x38,

    /* U+006C "l" */
    0xff, 0xff, 0xff, 0xff, 0xfe,

    /* U+006D "m" */
    0xee, 0xf7, 0xff, 0xf9, 0xcf, 0xce, 0x7e, 0x73,
    0xf3, 0x9f, 0x9c, 0xfc, 0xe7, 0xe7, 0x3f, 0x39,
    0xc0,

    /* U+006E "n" */
    0xef, 0x7f, 0xfc, 0xfc, 0x7e, 0x3f, 0x1f, 0x8f,
    0xc7, 0xe3, 0xf1, 0xc0,

    /* U+006F "o" */
    0x1e, 0x1f, 0xef, 0x3b, 0x87, 0xe1, 0xf8, 0x7e,
    0x1f, 0xce, 0x7f, 0x8f, 0xc0,

    /* U+0070 "p" */
    0xef, 0x3f, 0xef, 0x3b, 0x87, 0xe1, 0xf8, 0x7e,
    0x1f, 0xce, 0xff, 0xbb, 0xce, 0x3, 0x80, 0xe0,
    0x38, 0x0,

    /* U+0071 "q" */
    0x3d, 0xdf, 0xff, 0x3f, 0x87, 0xe1, 0xf8, 0x7e,
    0x1f, 0xcf, 0x7f, 0xcf, 0x70, 0x1c, 0x7, 0x1,
    0xc0, 0x70,

    /* U+0072 "r" */
    0xef, 0xff, 0x38, 0xe3, 0x8e, 0x38, 0xe3, 0x80,

    /* U+0073 "s" */
    0x1e, 0x1f, 0xe7, 0x19, 0xe0, 0x7f, 0x7, 0xf0,
    0x1d, 0xc7, 0x7f, 0x8f, 0xc0,

    /* U+0074 "t" */
    0x73, 0x9d, 0xff, 0xb9, 0xce, 0x73, 0x9c, 0xf3,
    0x80,

    /* U+0075 "u" */
    0xe3, 0xf1, 0xf8, 0xfc, 0x7e, 0x3f, 0x1f, 0x8f,
    0xc7, 0xff, 0xbd, 0xc0,

    /* U+0076 "v" */
    0xe1, 0x98, 0x67, 0x39, 0xcc, 0x33, 0xf, 0xc3,
    0xe0, 0x78, 0x1e, 0x7, 0x0,

    /* U+0077 "w" */
    0xe3, 0x1f, 0x9e, 0x66, 0x79, 0x99, 0xee, 0x77,
    0xb8, 0xd6, 0xc3, 0xcf, 0xf, 0x3c, 0x3c, 0xe0,
    0x73, 0x80,

    /* U+0078 "x" */
    0x73, 0x9c, 0xe3, 0xf0, 0x78, 0x1e, 0x7, 0x81,
    0xe0, 0xfc, 0x73, 0x9c, 0xe0,

    /* U+0079 "y" */
    0xc3, 0xf1, 0xb8, 0xcc, 0xe7, 0x63, 0xb0, 0xf8,
    0x78, 0x3c, 0xe, 0x6, 0x3, 0x7, 0x83, 0x80,

    /* U+007A "z" */
    0xff, 0xff, 0xf, 0xe, 0x1c, 0x38, 0x70, 0xe0,
    0xff, 0xff,

    /* U+007B "{" */
    0x1c, 0xf3, 0xc, 0x30, 0xc3, 0x3c, 0xf0, 0xc3,
    0xc, 0x30, 0xc3, 0xf, 0x1c,

    /* U+007C "|" */
    0xff, 0xff, 0x80,

    /* U+007D "}" */
    0xe3, 0xc3, 0xc, 0x30, 0xc3, 0xc, 0x3c, 0xf3,
    0xc, 0x30, 0xc3, 0x3c, 0xe0,

    /* U+007E "~" */
    0x71, 0x9f, 0xe,

    /* U+00B0 "°" */
    0x74, 0x63, 0x17, 0x0,

    /* U+00E8 "è" */
    0x30, 0xc, 0x3, 0x0, 0x3, 0xe3, 0xfb, 0x8d,
    0xc7, 0xff, 0xff, 0xf8, 0x1c, 0x77, 0xf1, 0xf0,

    /* U+00E9 "é" */
    0x6, 0x6, 0x2, 0x0, 0x3, 0xe3, 0xfb, 0x8d,
    0xc7, 0xff, 0xff, 0xf8, 0x1c, 0x77, 0xf1, 0xf0,

    /* U+00EA "ê" */
    0x1c, 0xb, 0x8, 0x80, 0x3, 0xe3, 0xfb, 0x8d,
    0xc7, 0xff, 0xff, 0xf8, 0x1c, 0x77, 0xf1, 0xf0};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 80, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 96, .box_w = 3, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 137, .box_w = 7, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 10, .adv_w = 160, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 28, .adv_w = 160, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 46, .adv_w = 256, .box_w = 15, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 71, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 69, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 91, .adv_w = 96, .box_w = 5, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 102, .adv_w = 96, .box_w = 5, .box_h = 17, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 113, .adv_w = 112, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 117, .adv_w = 168, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 128, .adv_w = 80, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 130, .adv_w = 96, .box_w = 5, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 132, .adv_w = 80, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 80, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 142, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 157, .adv_w = 160, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 212, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 227, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 160, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 274, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 96, .box_w = 3, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 96, .box_w = 3, .box_h = 13, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 298, .adv_w = 168, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 168, .box_w = 9, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 316, .adv_w = 168, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 327, .adv_w = 176, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 281, .box_w = 17, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 376, .adv_w = 208, .box_w = 13, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 398, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 416, .adv_w = 208, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 454, .adv_w = 192, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 176, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 224, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 80, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 529, .adv_w = 160, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 542, .adv_w = 208, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 176, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 577, .adv_w = 240, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 599, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 617, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 639, .adv_w = 192, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 656, .adv_w = 224, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 679, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 697, .adv_w = 192, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 715, .adv_w = 176, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 733, .adv_w = 208, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 751, .adv_w = 192, .box_w = 12, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 771, .adv_w = 272, .box_w = 17, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 799, .adv_w = 192, .box_w = 12, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 192, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 837, .adv_w = 176, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 854, .adv_w = 96, .box_w = 5, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 865, .adv_w = 80, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 874, .adv_w = 96, .box_w = 5, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 885, .adv_w = 168, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 893, .adv_w = 160, .box_w = 11, .box_h = 2, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 896, .adv_w = 96, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 11},
    {.bitmap_index = 898, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 911, .adv_w = 176, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 928, .adv_w = 160, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 940, .adv_w = 176, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 957, .adv_w = 160, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 969, .adv_w = 96, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 978, .adv_w = 176, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 994, .adv_w = 176, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1009, .adv_w = 80, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1014, .adv_w = 80, .box_w = 4, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1023, .adv_w = 160, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1038, .adv_w = 80, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1043, .adv_w = 256, .box_w = 13, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1060, .adv_w = 176, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1072, .adv_w = 176, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1085, .adv_w = 176, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1103, .adv_w = 176, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1121, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1129, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1142, .adv_w = 96, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1151, .adv_w = 176, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1163, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1176, .adv_w = 224, .box_w = 14, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1194, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1207, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1223, .adv_w = 144, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1233, .adv_w = 112, .box_w = 6, .box_h = 17, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1246, .adv_w = 81, .box_w = 1, .box_h = 17, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 1249, .adv_w = 112, .box_w = 6, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1262, .adv_w = 168, .box_w = 8, .box_h = 3, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 1265, .adv_w = 175, .box_w = 5, .box_h = 5, .ofs_x = 3, .ofs_y = 7},
    {.bitmap_index = 1269, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1285, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1301, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0}};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x38, 0x39, 0x3a};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
    {
        {.range_start = 32, .range_length = 95, .glyph_id_start = 1, .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY},
        {.range_start = 176, .range_length = 59, .glyph_id_start = 96, .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY}};

/*-----------------
 *    KERNING
 *----------------*/

/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 2, 3, 4, 4, 5, 6,
        4, 7, 7, 8, 9, 10, 11, 11,
        12, 13, 12, 14, 15, 16, 0, 17,
        17, 9, 17, 9, 0, 0, 0, 0,
        0, 0, 18, 19, 19, 20, 21, 22,
        23, 24, 25, 20, 26, 25, 27, 27,
        28, 19, 23, 29, 30, 31, 23, 32,
        33, 26, 33, 34, 0, 0, 0, 0,
        0, 21, 21, 21};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 2, 0, 3, 4,
        2, 5, 6, 7, 4, 3, 5, 5,
        2, 0, 2, 0, 8, 9, 0, 10,
        10, 11, 10, 11, 0, 0, 0, 0,
        0, 0, 12, 0, 13, 14, 15, 16,
        12, 17, 17, 18, 0, 17, 19, 19,
        13, 20, 14, 21, 22, 16, 23, 24,
        24, 25, 24, 26, 0, 0, 0, 0,
        0, 15, 15, 15};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
    {
        -17, -6, 0, 0, 0, 0, -6, -12,
        -12, -17, -12, -6, 0, 0, 0, -12,
        0, -6, -6, -6, -6, -9, -6, -12,
        -13, -6, 0, -12, -14, -16, -12, -9,
        0, -9, -12, -17, -3, -6, -6, -6,
        -4, -9, 0, 0, -9, -6, -9, -6,
        -7, -9, 0, -9, -9, -9, -14, -16,
        -12, -9, 0, -6, -14, -14, -14, -3,
        0, 0, 0, -3, 0, -6, -9, -3,
        -6, -3, -6, -9, -9, -9, -12, -6,
        -13, -14, -12, -9, 0, -3, -12, -14,
        -12, -6, 0, 0, 0, -6, 0, -9,
        -9, -6, -6, -6, -9, -3, -9, -9,
        -9, -12, -13, -14, -12, -9, -6, -14,
        -6, -14, -9, -9, -9, -9, -6, -9,
        0, -9, -12, -6, -9, -6, -13, -12,
        -9, -12, -12, -9, -14, -16, -12, -9,
        -9, -12, 0, -6, -9, -9, -9, -9,
        -6, -6, 0, -6, -14, -9, -12, -12,
        -14, -3, -12, -14, -6, -13, -13, -14,
        -12, -3, -6, -10, -3, -6, -17, -12,
        -9, -9, -6, -3, 0, -6, -14, -7,
        -10, -14, -13, -9, -16, -14, -9, -9,
        -13, -16, -12, -6, 0, -9, 0, -14,
        -14, -9, -6, -6, -4, 0, 0, -6,
        -12, -6, -9, -6, -12, -9, -14, -12,
        0, -14, -14, -16, -14, -12, 0, -14,
        0, 0, 0, -9, -12, -12, -9, -9,
        0, -3, -12, -9, -12, -6, -12, -9,
        -6, -12, 0, -12, -14, -16, -12, -9,
        0, -12, -12, -17, 0, -6, -9, -9,
        -7, -9, 0, -3, -12, 0, -9, -6,
        -12, -12, 0, -12, -12, -6, -12, -13,
        -9, -6, -3, -9, -6, -12, -14, -3,
        -6, -6, -6, -3, 0, -6, -9, -9,
        -9, -9, -9, -9, -12, -9, -9, 0,
        -9, -12, -9, -3, 0, -6, -12, -12,
        -14, -6, 0, 0, 0, 0, 0, 0,
        -6, -3, -6, 0, -4, 0, 0, -6,
        -17, -6, -13, -14, -12, -6, -12, -9,
        -9, -12, -17, -14, -12, -12, -9, 0,
        0, 0, -12, -6, -9, -9, -12, 0,
        -12, -12, -3, -3, -12, -16, -9, -3,
        0, -6, -6, -14, -6, -3, -3, -3,
        0, 0, 0, -3, -3, -3, -6, -3,
        -12, -3, 0, -3, -9, -6, -10, -12,
        -9, -6, 0, -3, -12, -14, -14, 0,
        0, 0, 0, -3, 0, -3, -6, -3,
        -6, 0, -7, -6, -12, -6, -14, -12,
        -10, -12, -9, -6, -12, -12, 0, -3,
        -12, -12, -12, -12, -7, -6, 0, 0,
        -12, -9, -12, -12, -12, -6, -9, -12,
        -17, -14, -16, -17, -14, -12, -9, -14,
        0, 0, -9, -14, -14, -14, -9, -6,
        0, 0, -17, -12, -17, -12, -14, 0,
        -9, -17, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -3, -3, 0,
        0, -6, -4, 0, -3, 0, -3, -3,
        -3, -6, -4, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -3,
        -1, 0, 0, -4, -4, 0, -6, -1,
        -3, -4, -3, -3, -7, -3, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -3, -7, -6, -6, -3, -6, -1,
        -1, -3, -3, -6, -3, -6, -6, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -1, -1, 0, 0, -1,
        -3, 0, -4, 0, -1, -3, -1, -4,
        -7, -1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -4, -6, -3,
        -1, 0, -4, 0, -3, 0, -1, -6,
        -1, 0, -6, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -4,
        0, -4, -3, -4, -6, 0, 0, -1,
        -3, -3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -4, -6, -4, -3, -1, -7, -6,
        -7, -3, -6, -7, -6, -9, -6, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -7, -6, -4, -3, -7,
        -4, 0, 0, 0, 0, -3, 0, -7,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -4, -7, -9,
        -7, -3, -10, 0, -6, -3, -6, -3,
        -6, -6, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -3,
        -1, -3, -1, -3, -6, -4, -6, -1,
        -4, -4, -4, -10, -4, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -4, -3, -1, 0, -6, -9, -3,
        -9, -3, -4, -6, -4, -4, -9, -4,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -6, -4, -1, 0, 1,
        0, 0, -1, 0, 0, -3, -1, 0,
        -3, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -3, -3, -1,
        0, -6, -6, -6, -6, -3, -6, -3,
        -4, -6, -12, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -3,
        -7, -4, -1, -4, -4, 0, -3, 0,
        -3, -3, 0, 0, -6, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -7, -6, -7, -4, -3, -7, -6,
        -12, -9, -7, -6, -9, 0, -6, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -6, -3, -6, -3, -1,
        -6, -4, -9, -4, -7, -6, -7, -4,
        -4, -1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -6, -6, 0,
        0, -3, -6, -3, -9, -6, -9, -6,
        -7, -6, -6, 0};

/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
    {
        .class_pair_values = kern_class_values,
        .left_class_mapping = kern_left_class_mapping,
        .right_class_mapping = kern_right_class_mapping,
        .left_class_cnt = 34,
        .right_class_cnt = 26,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

extern const lv_font_t lv_font_montserrat_18;

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t freesans_18 = {
#else
lv_font_t freesans_18 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt, /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt, /*Function pointer to get glyph's bitmap*/
    .line_height = 18,                              /*The maximum line height required by the font*/
    .base_line = 4,                                 /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -3,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc, /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_montserrat_18,
#endif
    .user_data = NULL,
};

#endif /*#if FREESANS_18*/
