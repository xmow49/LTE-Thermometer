#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#include "lvgl.h"



#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_PLUG
#define LV_ATTRIBUTE_IMG_PLUG
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_PLUG uint8_t plug_map[] = {
  0x06, 0x06, 0x00, 
  0x06, 0x06, 0x00, 
  0x06, 0x06, 0x00, 
  0x06, 0x06, 0x00, 
  0x06, 0x06, 0x00, 
  0x06, 0x06, 0x00, 
  0x06, 0x06, 0x00, 
  0x0e, 0x07, 0x00, 
  0x1f, 0xff, 0x80, 
  0x1f, 0xff, 0x80, 
  0x1f, 0xff, 0x80, 
  0x1f, 0xff, 0x80, 
  0x1f, 0xff, 0x80, 
  0x1f, 0xff, 0x80, 
  0x1f, 0xff, 0x80, 
  0x0f, 0xff, 0x00, 
  0x0f, 0xff, 0x00, 
  0x07, 0xfe, 0x00, 
  0x01, 0xf8, 0x00, 
  0x00, 0xf0, 0x00, 
};

const lv_img_dsc_t plug = {
  .header.cf = LV_IMG_CF_ALPHA_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 20,
  .header.h = 20,
  .data_size = 60,
  .data = plug_map,
};