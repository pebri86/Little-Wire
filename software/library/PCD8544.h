/*
=================================================================================
 Name        : PCD8544.h
 Version     : 0.1

 Copyright (C) 2010 Limor Fried, Adafruit Industries
 CORTEX-M3 version by Le Dang Dung, 2011 LeeDangDung@gmail.com (tested on LPC1769)
 Raspberry Pi version by Andre Wussow, 2012, desk@binerry.de
 littleWire version by Fuji Pebri, 2016, pebri86@gmail.com (tested littleWire version 1.3)

 Description : PCD8544 LCD library!

================================================================================
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
================================================================================
 */


#include "littleWire.h"
#include "littleWire_util.h"
#include <stdint.h>
#include <stdbool.h>

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

// buffer for SPI Comms
unsigned char sendBuffer[1];
unsigned char receiveBuffer[1];

void LCDInit(littleWire *lwHandle, uint8_t DC, uint8_t RST, uint8_t contrast);
void LCDcommand(littleWire *lwHandle,uint8_t c);
void LCDsetContrast(littleWire *lwHandle,uint8_t val);
void LCDclear();
void LCDdisplay(littleWire *lwHandle);
void LCDsetPixel(uint8_t x, uint8_t y, uint8_t color);
uint8_t LCDgetPixel(uint8_t x, uint8_t y);
void LCDfillcircle(uint8_t x0, uint8_t y0, uint8_t r,uint8_t color);
void LCDdrawcircle(uint8_t x0, uint8_t y0, uint8_t r,uint8_t color);
void LCDdrawCircleHelper(uint8_t x0, uint8_t y0, uint8_t r, uint8_t cornername, uint8_t color);
void LCDfillCircleHelper(uint8_t x0, uint8_t y0, uint8_t r, uint8_t cornername, uint8_t delta, uint8_t color);
void LCDdrawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,uint8_t color);
void LCDdrawRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
void LCDfillRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
void LCDfillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,uint8_t color);
void LCDdrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void LCDfillTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void LCDdrawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void LCDdrawFastVLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
void LCDdrawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
void LCDsetCursor(uint8_t x, uint8_t y);
void LCDsetTextSize(uint8_t s);
void LCDsetTextColor(uint8_t c, uint8_t b);
void LCDwrite(uint8_t c);
void LCDshowLogo();
uint8_t LCDwidth(void);
uint8_t LCDheight(void);
void LCDdrawchar(uint8_t x, uint8_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);
void LCDdrawstring(uint8_t x, uint8_t line, char *c);
void LCDdrawstring_P(uint8_t x, uint8_t line, const char *c);
void LCDdrawbitmap(uint8_t x, uint8_t y,  const uint8_t *bitmap, uint8_t w, uint8_t h,  uint8_t color);
void LCDspiwrite(littleWire *lwHandle,uint8_t c);
void LCDsetTextWrap(bool w);
