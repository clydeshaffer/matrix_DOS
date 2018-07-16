#ifndef KEYB_H
#define KEYB_H

void init_keyboard(void);

void deinit_keyboard(void);

int update_keystates(int keystates, int* key_map, int key_map_size);

void disable_repeat();

void get_keys_hit(char *keybuf);

void clear_keybuf(char *keybuf);

int test_keybuf(char *keybuf, int keycode);

#endif