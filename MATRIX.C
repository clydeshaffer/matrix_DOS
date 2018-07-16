#include <stdio.h>
#include <dos.h>

#include "keyb.h"
#include "keycodes.h"

#include "txtui.h"

#define VIDEO_INT 0x10
#define SET_MODE 0x00
#define VGA_256_COLOR_MODE 0x13
#define VGA_TEXT_MODE 0x03


#define INPUT_STATUS_1 0x03da
#define VRETRACE 0x08

typedef unsigned char byte;

void wait_retrace() {
    while ((inp(INPUT_STATUS_1) & VRETRACE));
    while (!(inp(INPUT_STATUS_1) & VRETRACE));
}

void putGVal(unsigned char g) {
	outp(0x03c9, 0x00);
	outp(0x03c9, g);
	outp(0x03c9, 0x00);
}

void set_mode(byte mode)
{
    union REGS regs;
    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    int86(VIDEO_INT, &regs, &regs);
}


void green_palette() {
    unsigned char g, gstep;
    int i;
    outp(0x03c8, 0);

    g = 0x00;
    gstep = 0x3F >> 4;
    for(i = 0; i < 6; i++) {
	putGVal(g);
	g += gstep;
    }
    outp(0x03c8, 20);
    putGVal(g);
    g += gstep;
    outp(0x03c8, 7);
    putGVal(g);
    g += gstep;
    outp(0x03c8, 56);
    for(i = 0; i < 7; i ++) {
	putGVal(g);
	g += gstep;
    }
    outp(0x03c9, 0x3F);
    outp(0x03c9, 0x3F);
    outp(0x03c9, 0x3F);
}

int is_whitespace(char c) {
	return ((c == 0) || (c == 32) || (c == 255));
}

char rnd_printable() {
	char rndchr = (((char) rand()) % 253) + 1;
	if(rndchr >= 32){
		rndchr++;
	}
	return rndchr;
}

void step(int spawn) {
	int row, col;
	unsigned char currentColor;
	unsigned char aboveColor;
	unsigned char aboveChar;
	for(row = 0; row < 25; row++) {
	 for(col = 0; col < 80; col++) {
		currentColor = 0x0F & colorat(col, row);
		if(currentColor > 0) {
			colorat(col, row) = currentColor - 1;
		}
	 }
	}
	for(row = 1; row < 25; row++) {
	 for(col = 0; col < 80; col++) {
		currentColor = 0x0F & colorat(col, row);
		aboveColor = 0x0F & colorat(col, row-1);
		aboveChar = charat(col, row-1);
		if((aboveColor == 0x0E) && !is_whitespace(aboveChar)) {
			colorat(col, row) = 0x0F;
			charat(col, row) = rnd_printable();
		}
	 }
	}
	row = 0;
	for(col = 0; col < 80; col++) {
		currentColor = colorat(col, row);
		if(currentColor == 0 && ((rand() % 100) >= spawn)) {
			colorat(col, row) = 0x0F;
			charat(col, row) = rnd_printable();
		}
	}
}

int main(int argc, char** argv) {
	char keybuf[32];
	int i;
	init_keyboard();
	clear_keybuf(keybuf);
	green_palette();
	paint_box(0,0,80,25,0x0F);
	paint_box(0,0,80,1, 0x00);
	while(!test_keybuf(keybuf, KEY_ESC)) {
		step(95);
		wait_retrace();
		wait_retrace();
		wait_retrace();
		wait_retrace();
		get_keys_hit(keybuf);
	}
	deinit_keyboard();
	for(i = 0; i < 41; i ++) {
		step(100);
		wait_retrace();
		wait_retrace();
		wait_retrace();
		wait_retrace();
	}
	clear_screen(0x07);
	set_mode(VGA_TEXT_MODE);
	wait_retrace();
	return 0;
}