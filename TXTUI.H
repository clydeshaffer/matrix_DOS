#ifndef TXTUI_H
#define TXTUI_H

#define charat(x,y) txtmem[((y<<4)+(y<<6)+x)<<1]
#define colorat(x,y) txtmem[(((y<<4)+(y<<6)+x)<<1) + 1]

void clear_screen(char color);

void write_string(char* s, int x, int y, int len, char color);

void draw_box(int x, int y, int width, int height, char color);

void paint_box(int x, int y, int width, int height, char color);

void clear_box(int x, int y, int width, int height);

extern char far *txtmem;

#endif

