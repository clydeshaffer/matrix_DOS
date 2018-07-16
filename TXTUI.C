#include "txtui.h"

char far *txtmem = (char *) 0xB8000000L;

void clear_screen(char color) {
    int i;
    for(i = 0; i < 80 * 25; i++) {
        txtmem[i * 2] = 0;
        txtmem[i*2 + 1] = color;
    }
}

void write_string(char* s, int x, int y, int len, char color) {
    char far *ptr = &charat(x,y);
    while(len) {
        *ptr = *s;
        ptr++;
        s++;
        *ptr=color;
        ptr++;
        len--;
    }
}

void draw_box(int x, int y, int width, int height, char color) {
    int i;
    width--;
    height--;
    for(i = 1; i < width; i++) {
        charat(x+i, y) = 0xCD;
        charat(x+i, y+height) = 0xCD;
        colorat(x+i, y) = color;
        colorat(x+i, y+height) = color;
    }
    for(i = 1; i < height; i++) {
        charat(x, y+i) = 0xBA;
        charat(x+width, y+i) = 0xBA;
        colorat(x, y+i) = color;
        colorat(x+width, y+i) = color;
    }
    charat(x, y) = 0xC9;
    charat(x+width, y) = 0xBB;
    charat(x, y+height) = 0xC8;
    charat(x+width, y+height) = 0xBC;
    colorat(x, y) = color;
    colorat(x+width, y) = color;
    colorat(x, y+height) = color;
    colorat(x+width, y+height) = color;
}

void paint_box(int x, int y, int width, int height, char color) {
    int i, j;
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            colorat(x+i, y+j) = color;
        }
    }
}

void clear_box(int x, int y, int width, int height) {
    int i, j;
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            charat(x+i, y+j) = 0;
        }
    }
}

