#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"
#include "PCD8544.h"
#define min(a,b)(a<b?a:b)

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

static const unsigned char logo16_glcd_bmp[] = 
{
	0x00, 0x60, 0xE0, 0xE0, 0xE0, 0x60, 0x40, 0xF0, 0x38, 0xBC, 0xF8, 0x80, 0x80, 0x80, 0x80, 0x00, 
	0x00, 0x00, 0x38, 0x3D, 0x3F, 0x37, 0x1B, 0x1F, 0x73, 0xF5, 0xFC, 0x65, 0x07, 0x07, 0x07, 0x01 
};

// pin setup
int _dc = PIN3;
int _rst = PIN4;

unsigned char version;

littleWire *lw = NULL;

// lcd contrast
int contrast = 50;

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h);
void testdrawchar(void);
void testdrawcircle(void);
void testfillrect(void);
void testdrawroundrect(void);
void testfillroundrect(void);
void testdrawrect(void);
void testdrawline();
void testdrawtriangle(void);
void testfilltriangle(void);

int main()
{ 

	lw = littleWire_connect();

	if(lw == NULL)
	{
		printf("> Little Wire could not be found!\n");
		exit(EXIT_FAILURE);
	}

	version = readFirmwareVersion(lw);
	printf("> Little Wire firmware version: %d.%d\n",((version & 0xF0)>>4),(version&0x0F));
	if(version==0x10)
	{
		printf("> This example requires the new 1.1 version firmware. Please update soon.\n");
		return 0;
	}

	spi_updateDelay(lw, 0);

		// init and clear lcd
	LCDInit(lw, _dc, _rst, contrast);
	LCDclear(lw);

	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// draw a single pixel
	LCDsetPixel(10, 10, BLACK);
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// draw many lines
	testdrawline();
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// draw rectangles
	testdrawrect();
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// draw multiple rectangles
	testfillrect();
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// draw mulitple circles
	testdrawcircle();
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// draw a circle, 10 pixel radius
	LCDfillcircle(LCDwidth()/2, LCDheight()/2, 10, BLACK);
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	testdrawroundrect();
	delay(2000);
	LCDclear();

	testfillroundrect();
	delay(2000);
	LCDclear();

	testdrawtriangle();
	delay(2000);
	LCDclear();

	testfilltriangle();
	delay(2000);
	LCDclear();

	testdrawchar();
	LCDdisplay(lw);
	delay(2000);
	LCDclear();

	// text display tests
	LCDsetTextSize(1);
	LCDsetTextColor(BLACK, WHITE);
	LCDsetCursor(0,0);
	LCDdrawstring(0,0, "Hello, world!");
	LCDdisplay(lw);
	LCDclear();
	LCDsetTextSize(2);
	LCDsetTextColor(BLACK, WHITE);
	LCDdrawstring(0,2, "Hello Again!");
	LCDdisplay(lw);
	delay(2000);

	// miniature bitmap display
	LCDclear();
	LCDdrawbitmap(30, 16, logo16_glcd_bmp, 16, 16, BLACK);
	LCDdisplay(lw);

	// draw a bitmap icon and 'animate' movement
	testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT);
}

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) 
{
	uint8_t icons[NUMFLAKES][3];
	time_t t;

	 /* Intializes random number generator */
	srand((unsigned) time(&t));
	uint8_t f;
	
	// initialize
	for (f=0; f< NUMFLAKES; f++) 
	{
		icons[f][XPOS] = rand() % LCDwidth();
		icons[f][YPOS] = 0;
		icons[f][DELTAY] = rand() % (5 + 1);
	}

	while (1) 
	{
		uint8_t f;
		// draw each icon
		for (f=0; f< NUMFLAKES; f++) 
		{
			LCDdrawbitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
		}
		LCDdisplay(lw);
		delay(200);
		
		// then erase it + move it
		for (f=0; f< NUMFLAKES; f++) 
		{
			LCDdrawbitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, WHITE);
			// move it
			icons[f][YPOS] += icons[f][DELTAY];
			// if its gone, reinit
			if (icons[f][YPOS] > LCDheight()) {
				icons[f][XPOS] = rand() % LCDwidth();
				icons[f][YPOS] = 0;
				icons[f][DELTAY] = rand() % (5+1);
			}
		}
	}
}

void testdrawchar(void) 
{
	LCDsetTextSize(1);
	LCDsetTextColor(BLACK, WHITE);
	LCDsetCursor(0,0);
	uint8_t i;

	printf("Test: Draw the first ~120 chars.\n");
	for (i=0; i < 64; i++)
	{
		LCDdrawchar((i % 14) * 6, (i/14) * 8, i, BLACK, WHITE, 1);
		LCDdisplay(lw);
	}
	delay(2000);
	LCDclear();
	LCDdisplay(lw);
	for (i=0; i < 64; i++)
	{
		LCDdrawchar((i % 14) * 6, (i/14) * 8, i + 64, WHITE, BLACK, 1);
		LCDdisplay(lw);
	}
	delay(2000);
	LCDclear();
	LCDdisplay(lw);
}

void testdrawcircle(void) 
{
	int16_t i;
	for (i=0; i<LCDheight(); i+=2) 
	{
		LCDdrawcircle(LCDwidth()/2, LCDheight()/2, i, BLACK);
		LCDdisplay(lw);
	}
}

void testfillrect(void) 
{
	uint8_t color = 1;
	int16_t i;
	for (i=0; i<LCDheight()/2; i+=3) 
	{
		// alternate colors
		LCDfillrect(i, i, LCDwidth()-i*2, LCDheight()-i*2, color%2);
		LCDdisplay(lw);
		color++;
	}
}

void testdrawtriangle(void) 
{
	uint8_t i;
	for (i=0; i<min(LCDwidth(),LCDheight())/2; i+=5) 
	{
		LCDdrawTriangle(LCDwidth()/2, LCDheight()/2-i,
			LCDwidth()/2-i, LCDheight()/2+i,
			LCDwidth()/2+i, LCDheight()/2+i, BLACK);
		LCDdisplay(lw);
	}
}

void testfilltriangle(void) 
{
	uint8_t color = BLACK;
	uint8_t i;
	for (i=0; i<min(LCDwidth(),LCDheight())/2; i+=5) 
	{
		LCDfillTriangle(LCDwidth()/2, LCDheight()/2-i,
			LCDwidth()/2-i, LCDheight()/2+i,
			LCDwidth()/2+i, LCDheight()/2+i, color);
		if (color == WHITE) color = BLACK;
		else color = WHITE;
		LCDdisplay(lw);
	}
}

void testdrawroundrect(void) 
{
	int16_t i;
	for (i=0; i<LCDheight()/2-2; i+=2) 
	{
		LCDdrawRoundRect(i, i, LCDwidth()-2*i, LCDheight()-2*i, LCDheight()/4, BLACK);
		LCDdisplay(lw);
	}
}

void testfillroundrect(void) 
{
	uint8_t color = BLACK;
	int16_t i;
	for (i=0; i<LCDheight()/2-2; i+=2) 
	{
		LCDfillRoundRect(i, i, LCDwidth()-2*i, LCDheight()-2*i, LCDheight()/4, color);
		if (color == WHITE) color = BLACK;
		else color = WHITE;
		LCDdisplay(lw);
	}
}

void testdrawrect(void) 
{
	int16_t i;
	for (i=0; i<LCDheight()/2; i+=2) 
	{
		LCDdrawrect(i, i, LCDwidth()-2*i, LCDheight()-2*i, BLACK);
		LCDdisplay(lw);
	}
}

void testdrawline() 
{

	int16_t i;

	for (i=0; i<LCDwidth(); i+=4) 
	{
		LCDdrawline(0, 0, i, LCDheight()-1, BLACK);
		LCDdisplay(lw);
	}
	for (i=0; i<LCDheight(); i+=4) 
	{
		LCDdrawline(0, 0, LCDwidth()-1, i, BLACK);
		LCDdisplay(lw);
	}
	delay(250);
	
	LCDclear();
	for (i=0; i<LCDwidth(); i+=4) 
	{
		LCDdrawline(0, LCDheight()-1, i, 0, BLACK);
		LCDdisplay(lw);
	}
	for (i=LCDheight()-1; i>=0; i-=4) 
	{
		LCDdrawline(0, LCDheight()-1, LCDwidth()-1, i, BLACK);
		LCDdisplay(lw);
	}
	delay(250);
	
	LCDclear();
	for (i=LCDwidth()-1; i>=0; i-=4) 
	{
		LCDdrawline(LCDwidth()-1, LCDheight()-1, i, 0, BLACK);
		LCDdisplay(lw);
	}
	for (i=LCDheight()-1; i>=0; i-=4) 
	{
		LCDdrawline(LCDwidth()-1, LCDheight()-1, 0, i, BLACK);
		LCDdisplay(lw);
	}
	delay(250);

	LCDclear();
	for (i=0; i<LCDheight(); i+=4) 
	{
		LCDdrawline(LCDwidth()-1, 0, 0, i, BLACK);
		LCDdisplay(lw);
	}
	for (i=0; i<LCDwidth(); i+=4) 
	{
		LCDdrawline(LCDwidth()-1, 0, i, LCDheight()-1, BLACK); 
		LCDdisplay(lw);
	}
	delay(250);
}
