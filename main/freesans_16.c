/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font FreeSansBold.ttf --symbols °éèê% --range 32-127 --format lvgl -o freesans_16.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef FREESANS_16
#define FREESANS_16 1
#endif

#if FREESANS_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf,

    /* U+0022 "\"" */
    0xcf, 0x3c, 0xf3,

    /* U+0023 "#" */
    0x1b, 0xd, 0x84, 0x8f, 0xf7, 0xf9, 0xb0, 0xd9,
    0xfe, 0xff, 0x12, 0x1b, 0xd, 0x86, 0xc0,

    /* U+0024 "$" */
    0x8, 0x3e, 0xff, 0xcb, 0xc8, 0xf8, 0x3e, 0xf,
    0xb, 0xcb, 0xff, 0x7e, 0x8, 0x8,

    /* U+0025 "%" */
    0x78, 0x43, 0xf3, 0xc, 0xc8, 0x33, 0x60, 0xfd,
    0x1, 0xec, 0x0, 0x27, 0x1, 0x7f, 0x5, 0x8c,
    0x26, 0x30, 0x9f, 0xc4, 0x1c,

    /* U+0026 "&" */
    0x3c, 0x1f, 0x86, 0x61, 0x98, 0x3c, 0xe, 0x7,
    0xdb, 0x3e, 0xc7, 0x39, 0xcf, 0xf8, 0xf7,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x33, 0x66, 0xcc, 0xcc, 0xcc, 0xcc, 0x66, 0x33,

    /* U+0029 ")" */
    0xcc, 0x66, 0x23, 0x33, 0x33, 0x33, 0x66, 0xcc,

    /* U+002A "*" */
    0x25, 0x7e, 0xe5, 0x0,

    /* U+002B "+" */
    0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0xf5, 0x80,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x11, 0x12, 0x22, 0x24, 0x44, 0xc8,

    /* U+0030 "0" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0031 "1" */
    0x18, 0xff, 0xf1, 0x8c, 0x63, 0x18, 0xc6, 0x30,

    /* U+0032 "2" */
    0x3c, 0x7e, 0xe7, 0xc3, 0x3, 0x7, 0xe, 0x1c,
    0x70, 0x60, 0xff, 0xff,

    /* U+0033 "3" */
    0x3c, 0x7e, 0xc6, 0xc6, 0x6, 0x1c, 0x1e, 0x7,
    0xc3, 0xc7, 0x7e, 0x3c,

    /* U+0034 "4" */
    0xe, 0xe, 0x1e, 0x36, 0x26, 0x66, 0xc6, 0xff,
    0xff, 0x6, 0x6, 0x6,

    /* U+0035 "5" */
    0x7f, 0x7f, 0x40, 0x40, 0x5c, 0xfe, 0xc7, 0x3,
    0x3, 0xc7, 0x7e, 0x3c,

    /* U+0036 "6" */
    0x3c, 0x7e, 0x63, 0xc0, 0xdc, 0xfe, 0xe7, 0xc3,
    0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0037 "7" */
    0xff, 0xff, 0x6, 0x6, 0xc, 0x1c, 0x18, 0x18,
    0x30, 0x30, 0x30, 0x30,

    /* U+0038 "8" */
    0x3c, 0xff, 0xc3, 0xc3, 0x7e, 0x7e, 0xe7, 0xc3,
    0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x7e, 0xe6, 0xc3, 0xc3, 0xe7, 0x7f, 0x3b,
    0x3, 0xe6, 0xfe, 0x3c,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0x3, 0xd6,

    /* U+003C "<" */
    0x1, 0x7, 0x3e, 0xf0, 0xe0, 0x7c, 0xf, 0x3,

    /* U+003D "=" */
    0xff, 0xff, 0x0, 0x0, 0xff, 0xff,

    /* U+003E ">" */
    0x80, 0xe0, 0x7c, 0xf, 0x7, 0x1e, 0xf8, 0xc0,

    /* U+003F "?" */
    0x3c, 0x7e, 0xe7, 0xc3, 0x3, 0x7, 0xe, 0x1c,
    0x18, 0x0, 0x18, 0x18,

    /* U+0040 "@" */
    0x7, 0xe0, 0x3f, 0xf0, 0xe0, 0xf3, 0xbf, 0x66,
    0xce, 0x79, 0x8, 0xf6, 0x11, 0xec, 0x63, 0xd8,
    0xcd, 0xbe, 0xf9, 0xb9, 0xc3, 0xc0, 0x3, 0xfe,
    0x1, 0xf8, 0x0,

    /* U+0041 "A" */
    0xe, 0x1, 0xe0, 0x3c, 0xf, 0x81, 0xb8, 0x33,
    0xe, 0x61, 0x8e, 0x3f, 0xcf, 0xf9, 0x83, 0xb0,
    0x30,

    /* U+0042 "B" */
    0xfe, 0x7f, 0xb0, 0x78, 0x3c, 0x1f, 0xfb, 0xfd,
    0x83, 0xc1, 0xe0, 0xff, 0xdf, 0xc0,

    /* U+0043 "C" */
    0x1f, 0xf, 0xe6, 0x1f, 0x87, 0xc0, 0x30, 0xc,
    0x3, 0x0, 0xe1, 0xd8, 0x77, 0xf8, 0x7c,

    /* U+0044 "D" */
    0xfe, 0x7f, 0xb0, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe1, 0xbf, 0xdf, 0xc0,

    /* U+0045 "E" */
    0xff, 0xff, 0xf0, 0x18, 0xc, 0x7, 0xfb, 0xfd,
    0x80, 0xc0, 0x60, 0x3f, 0xff, 0xf0,

    /* U+0046 "F" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xff, 0xff, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x1f, 0x87, 0xf9, 0xc3, 0xf0, 0x3c, 0x1, 0x87,
    0xf0, 0xfe, 0x3, 0xe0, 0xee, 0x3c, 0xff, 0x8f,
    0xb0,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1f, 0xff, 0xff,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0xe3, 0xc7,
    0x8f, 0xfb, 0xe0,

    /* U+004B "K" */
    0xc3, 0xb1, 0xcc, 0xe3, 0x30, 0xd8, 0x3e, 0xf,
    0xc3, 0x38, 0xce, 0x31, 0xcc, 0x3b, 0x7,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xff, 0xff,

    /* U+004D "M" */
    0xe0, 0xfe, 0x3f, 0xc7, 0xf8, 0xff, 0x1f, 0xb6,
    0xf6, 0xde, 0xdb, 0xdb, 0x79, 0xcf, 0x39, 0xe7,
    0x30,

    /* U+004E "N" */
    0xc1, 0xf0, 0xfc, 0x7e, 0x3f, 0x9e, 0xcf, 0x37,
    0x9f, 0xc7, 0xe3, 0xf0, 0xf8, 0x30,

    /* U+004F "O" */
    0x1f, 0x7, 0xf1, 0xc7, 0x70, 0x7c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xe0, 0xee, 0x38, 0xfe, 0xf,
    0x80,

    /* U+0050 "P" */
    0xfe, 0x7f, 0xb0, 0xf8, 0x3c, 0x1e, 0x1f, 0xfd,
    0xfc, 0xc0, 0x60, 0x30, 0x18, 0x0,

    /* U+0051 "Q" */
    0x1f, 0x7, 0xf1, 0xc7, 0x70, 0x7c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xe3, 0xee, 0x38, 0xff, 0xf,
    0xb0, 0x0,

    /* U+0052 "R" */
    0xff, 0x7f, 0xf0, 0x78, 0x3c, 0x1f, 0xfb, 0xfd,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30,

    /* U+0053 "S" */
    0x3e, 0x3f, 0xb8, 0xf8, 0x3e, 0x3, 0xf0, 0x7c,
    0x3, 0xc1, 0xf1, 0xdf, 0xc7, 0xc0,

    /* U+0054 "T" */
    0xff, 0xff, 0xc3, 0x1, 0x80, 0xc0, 0x60, 0x30,
    0x18, 0xc, 0x6, 0x3, 0x1, 0x80,

    /* U+0055 "U" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xf1, 0xdf, 0xc7, 0xc0,

    /* U+0056 "V" */
    0x60, 0xd8, 0x37, 0x1c, 0xc6, 0x31, 0x8c, 0xe1,
    0xb0, 0x6c, 0x1b, 0x3, 0x80, 0xe0, 0x38,

    /* U+0057 "W" */
    0xe3, 0x8e, 0xc7, 0x1d, 0x8e, 0x33, 0x14, 0x67,
    0x6d, 0xc6, 0xdb, 0xd, 0xb6, 0x1b, 0x2c, 0x14,
    0x78, 0x38, 0xe0, 0x71, 0xc0, 0xe3, 0x80,

    /* U+0058 "X" */
    0x71, 0xdc, 0x63, 0xb8, 0x6c, 0x1f, 0x3, 0x80,
    0xe0, 0x7c, 0x3b, 0xc, 0xe7, 0x1d, 0x87,

    /* U+0059 "Y" */
    0xe1, 0x98, 0xe7, 0x30, 0xcc, 0x3f, 0x7, 0x81,
    0xe0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0xff, 0xc0, 0xe0, 0xe0, 0xe0, 0xe0, 0x70,
    0x70, 0x70, 0x70, 0x3f, 0xff, 0xf0,

    /* U+005B "[" */
    0xff, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xff,

    /* U+005C "\\" */
    0x84, 0x10, 0x84, 0x10, 0x84, 0x30, 0x84, 0x20,

    /* U+005D "]" */
    0xff, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xff,

    /* U+005E "^" */
    0x38, 0x70, 0xb3, 0x66, 0x48, 0xf1, 0x80,

    /* U+005F "_" */
    0xff, 0xff, 0xf0,

    /* U+0060 "`" */
    0xcc, 0x80,

    /* U+0061 "a" */
    0x7c, 0x7e, 0xc6, 0x6, 0x7e, 0xc6, 0xc6, 0xfe,
    0x76,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xdc, 0xfe, 0xe7, 0xc3, 0xc3,
    0xc3, 0xe7, 0xfe, 0xdc,

    /* U+0063 "c" */
    0x3c, 0x7e, 0xe6, 0xc0, 0xc0, 0xc0, 0xe6, 0x7e,
    0x3c,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3b, 0x7f, 0xe7, 0xc3, 0xc3,
    0xc3, 0xe7, 0x7f, 0x3b,

    /* U+0065 "e" */
    0x3c, 0x7e, 0xe7, 0xff, 0xff, 0xc0, 0xe7, 0x7e,
    0x3c,

    /* U+0066 "f" */
    0x3b, 0xd9, 0xff, 0xb1, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0067 "g" */
    0x3b, 0x7f, 0xe7, 0xc3, 0xc3, 0xc3, 0xe7, 0x7f,
    0x3b, 0x3, 0xe7, 0xfe, 0x3c,

    /* U+0068 "h" */
    0xc1, 0x83, 0x6, 0xef, 0xf8, 0xf1, 0xe3, 0xc7,
    0x8f, 0x1e, 0x30,

    /* U+0069 "i" */
    0xf3, 0xff, 0xff,

    /* U+006A "j" */
    0x6c, 0x36, 0xdb, 0x6d, 0xb6, 0xff,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xce, 0xcc, 0xd8, 0xf0, 0xf8,
    0xdc, 0xcc, 0xce, 0xc6,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0xdc, 0xef, 0xff, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x30,

    /* U+006E "n" */
    0xde, 0xff, 0xe3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3,

    /* U+006F "o" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e,
    0x3c,

    /* U+0070 "p" */
    0xdc, 0xfe, 0xe7, 0xc3, 0xc3, 0xc3, 0xe7, 0xfe,
    0xdc, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3b, 0x7f, 0xe7, 0xc3, 0xc3, 0xc3, 0xe7, 0x7f,
    0x3b, 0x3, 0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xdf, 0xf9, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x3e, 0x7f, 0x63, 0x70, 0x3e, 0x7, 0x63, 0x7f,
    0x3e,

    /* U+0074 "t" */
    0x63, 0x19, 0xff, 0xb1, 0x8c, 0x63, 0x1e, 0x70,

    /* U+0075 "u" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xff,
    0x7b,

    /* U+0076 "v" */
    0xe3, 0x31, 0x98, 0xce, 0xc3, 0x61, 0xb0, 0xf0,
    0x38, 0x1c, 0x0,

    /* U+0077 "w" */
    0xe6, 0x3e, 0x73, 0x67, 0x76, 0xf6, 0x6d, 0x63,
    0xd6, 0x39, 0xc3, 0x9c, 0x39, 0xc0,

    /* U+0078 "x" */
    0x63, 0x3b, 0x8d, 0x83, 0x81, 0xc1, 0xe0, 0xd8,
    0xee, 0x63, 0x0,

    /* U+0079 "y" */
    0xc3, 0xe3, 0x66, 0x66, 0x76, 0x3c, 0x3c, 0x3c,
    0x1c, 0x18, 0x18, 0x78, 0x70,

    /* U+007A "z" */
    0xff, 0xfc, 0x38, 0xe3, 0x8e, 0x38, 0x7f, 0xfe,

    /* U+007B "{" */
    0x19, 0xcc, 0x63, 0x18, 0xde, 0xf1, 0x8c, 0x63,
    0x18, 0xe3,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff,

    /* U+007D "}" */
    0xe7, 0x8c, 0x63, 0x18, 0xc7, 0x39, 0x8c, 0x63,
    0x1b, 0xdc,

    /* U+007E "~" */
    0xf3, 0x3c, 0x30,

    /* U+00B0 "°" */
    0x74, 0x63, 0x17, 0x0,

    /* U+00E8 "è" */
    0x30, 0x10, 0x8, 0x0, 0x3c, 0x7e, 0xe7, 0xff,
    0xff, 0xc0, 0xe7, 0x7e, 0x3c,

    /* U+00E9 "é" */
    0xc, 0x8, 0x10, 0x0, 0x3c, 0x7e, 0xe7, 0xff,
    0xff, 0xc0, 0xe7, 0x7e, 0x3c,

    /* U+00EA "ê" */
    0x18, 0x34, 0x26, 0x0, 0x3c, 0x7e, 0xe7, 0xff,
    0xff, 0xc0, 0xe7, 0x7e, 0x3c};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 71, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 85, .box_w = 2, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 121, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 7, .adv_w = 142, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 22, .adv_w = 142, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 36, .adv_w = 228, .box_w = 14, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 185, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 61, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 73, .adv_w = 85, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 81, .adv_w = 85, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 89, .adv_w = 100, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 93, .adv_w = 150, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 101, .adv_w = 71, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 103, .adv_w = 85, .box_w = 4, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 104, .adv_w = 71, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 71, .box_w = 4, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 142, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 179, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 227, .adv_w = 85, .box_w = 2, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 230, .adv_w = 85, .box_w = 2, .box_h = 12, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 233, .adv_w = 150, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 150, .box_w = 8, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 247, .adv_w = 150, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 250, .box_w = 15, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 294, .adv_w = 185, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 185, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 380, .adv_w = 199, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 411, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 425, .adv_w = 185, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 440, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 213, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 469, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 483, .adv_w = 199, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 514, .adv_w = 199, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 532, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 546, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 560, .adv_w = 156, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 574, .adv_w = 185, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 171, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 603, .adv_w = 242, .box_w = 15, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 171, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 171, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 656, .adv_w = 156, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 670, .adv_w = 85, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 678, .adv_w = 71, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 686, .adv_w = 85, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 694, .adv_w = 150, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 701, .adv_w = 142, .box_w = 10, .box_h = 2, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 704, .adv_w = 85, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 706, .adv_w = 142, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 715, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 727, .adv_w = 142, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 736, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 748, .adv_w = 142, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 757, .adv_w = 85, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 765, .adv_w = 156, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 778, .adv_w = 156, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 789, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 792, .adv_w = 71, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 798, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 810, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 228, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 827, .adv_w = 156, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 836, .adv_w = 156, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 845, .adv_w = 156, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 858, .adv_w = 156, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 871, .adv_w = 100, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 877, .adv_w = 142, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 886, .adv_w = 85, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 894, .adv_w = 156, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 903, .adv_w = 142, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 914, .adv_w = 199, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 928, .adv_w = 142, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 939, .adv_w = 142, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 952, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 960, .adv_w = 100, .box_w = 5, .box_h = 16, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 970, .adv_w = 72, .box_w = 2, .box_h = 16, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 974, .adv_w = 100, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 984, .adv_w = 150, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 987, .adv_w = 155, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 991, .adv_w = 142, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1004, .adv_w = 142, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1017, .adv_w = 142, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0}};

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
        -15, -5, 0, 0, 0, 0, -5, -10,
        -10, -15, -10, -5, 0, 0, 0, -10,
        0, -5, -5, -5, -5, -8, -5, -10,
        -12, -5, 0, -10, -13, -14, -10, -8,
        0, -8, -10, -15, -3, -5, -5, -5,
        -4, -8, 0, 0, -8, -5, -8, -5,
        -6, -8, 0, -8, -8, -8, -13, -14,
        -10, -8, 0, -5, -13, -13, -13, -3,
        0, 0, 0, -3, 0, -5, -8, -3,
        -5, -3, -5, -8, -8, -8, -10, -5,
        -12, -13, -10, -8, 0, -3, -10, -13,
        -10, -5, 0, 0, 0, -5, 0, -8,
        -8, -5, -5, -5, -8, -3, -8, -8,
        -8, -10, -12, -13, -10, -8, -5, -13,
        -5, -13, -8, -8, -8, -8, -5, -8,
        0, -8, -10, -5, -8, -5, -12, -10,
        -8, -10, -10, -8, -13, -14, -10, -8,
        -8, -10, 0, -5, -8, -8, -8, -8,
        -5, -5, 0, -5, -13, -8, -10, -10,
        -13, -3, -10, -13, -5, -12, -12, -13,
        -10, -3, -5, -9, -3, -5, -15, -10,
        -8, -8, -5, -3, 0, -5, -13, -6,
        -9, -13, -12, -8, -14, -13, -8, -8,
        -12, -14, -10, -5, 0, -8, 0, -13,
        -13, -8, -5, -5, -4, 0, 0, -5,
        -10, -5, -8, -5, -10, -8, -13, -10,
        0, -13, -13, -14, -13, -10, 0, -13,
        0, 0, 0, -8, -10, -10, -8, -8,
        0, -3, -10, -8, -10, -5, -10, -8,
        -5, -10, 0, -10, -13, -14, -10, -8,
        0, -10, -10, -15, 0, -5, -8, -8,
        -6, -8, 0, -3, -10, 0, -8, -5,
        -10, -10, 0, -10, -10, -5, -10, -12,
        -8, -5, -3, -8, -5, -10, -13, -3,
        -5, -5, -5, -3, 0, -5, -8, -8,
        -8, -8, -8, -8, -10, -8, -8, 0,
        -8, -10, -8, -3, 0, -5, -10, -10,
        -13, -5, 0, 0, 0, 0, 0, 0,
        -5, -3, -5, 0, -4, 0, 0, -5,
        -15, -5, -12, -13, -10, -5, -10, -8,
        -8, -10, -15, -13, -10, -10, -8, 0,
        0, 0, -10, -5, -8, -8, -10, 0,
        -10, -10, -3, -3, -10, -14, -8, -3,
        0, -5, -5, -13, -5, -3, -3, -3,
        0, 0, 0, -3, -3, -3, -5, -3,
        -10, -3, 0, -3, -8, -5, -9, -10,
        -8, -5, 0, -3, -10, -13, -13, 0,
        0, 0, 0, -3, 0, -3, -5, -3,
        -5, 0, -6, -5, -10, -5, -13, -10,
        -9, -10, -8, -5, -10, -10, 0, -3,
        -10, -10, -10, -10, -6, -5, 0, 0,
        -10, -8, -10, -10, -10, -5, -8, -10,
        -15, -13, -14, -15, -13, -10, -8, -13,
        0, 0, -8, -13, -13, -13, -8, -5,
        0, 0, -15, -10, -15, -10, -13, 0,
        -8, -15, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -3, -3, 0,
        0, -5, -4, 0, -3, 0, -3, -3,
        -3, -5, -4, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -3,
        -1, 0, 0, -4, -4, 0, -5, -1,
        -3, -4, -3, -3, -6, -3, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -3, -6, -5, -5, -3, -5, -1,
        -1, -3, -3, -5, -3, -5, -5, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -1, -1, 0, 0, -1,
        -3, 0, -4, 0, -1, -3, -1, -4,
        -6, -1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -4, -5, -3,
        -1, 0, -4, 0, -3, 0, -1, -5,
        -1, 0, -5, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -4,
        0, -4, -3, -4, -5, 0, 0, -1,
        -3, -3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -4, -5, -4, -3, -1, -6, -5,
        -6, -3, -5, -6, -5, -8, -5, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -6, -5, -4, -3, -6,
        -4, 0, 0, 0, 0, -3, 0, -6,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -4, -6, -8,
        -6, -3, -9, 0, -5, -3, -5, -3,
        -5, -5, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -3,
        -1, -3, -1, -3, -5, -4, -5, -1,
        -4, -4, -4, -9, -4, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -4, -3, -1, 0, -5, -8, -3,
        -8, -3, -4, -5, -4, -4, -8, -4,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -5, -4, -1, 0, 1,
        0, 0, -1, 0, 0, -3, -1, 0,
        -3, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -3, -3, -1,
        0, -5, -5, -5, -5, -3, -5, -3,
        -4, -5, -10, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, -3,
        -6, -4, -1, -4, -4, 0, -3, 0,
        -3, -3, 0, 0, -5, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, -6, -5, -6, -4, -3, -6, -5,
        -10, -8, -6, -5, -8, 0, -5, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -5, -3, -5, -3, -1,
        -5, -4, -8, -4, -6, -5, -6, -4,
        -4, -1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, -5, -5, 0,
        0, -3, -5, -3, -8, -5, -8, -5,
        -6, -5, -5, 0};

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

extern const lv_font_t lv_font_montserrat_16;

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t freesans_16 = {
#else
lv_font_t freesans_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt, /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt, /*Function pointer to get glyph's bitmap*/
    .line_height = 17,                              /*The maximum line height required by the font*/
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
    .fallback = &lv_font_montserrat_16,
#endif
    .user_data = NULL,
};

#endif /*#if FREESANS_16*/
