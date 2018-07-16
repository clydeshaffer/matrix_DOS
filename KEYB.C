#include <dos.h>
#include "keyb.h"

#define BYTE unsigned char
#define NUM_SCAN_QUE 256     /* this MUST be 256, using BYTE roll-over for
                              q code */

#define KEY_RELEASED 128

/* the interrupt keyword causes the compiler to save all the registers before the 
function is called, and restore them on exit. It also makes the function return via a 
IRET.*/

static void interrupt (far *oldkb)(void);   /* BIOS keyboard handler */

/* Q code  */
BYTE gb_scan;
BYTE gb_scan_q[NUM_SCAN_QUE];
BYTE gb_scan_head;
BYTE gb_scan_tail;

/*
   invoked by the hardware keyboard interupt
   ques up the raw scan codes
    stuff raw scan codes into the array gb_scan_q[]
*/
/* ---------------------- get_scan() --------------------- April 17,1993 */
void interrupt get_scan(void)
{

   /* read the raw scan code from the keyboard */
   asm   cli

      asm   in    al, 060h       /* read scan code */
   asm      mov   gb_scan, al
   asm      in    al, 061h       /* read keyboard status */
   asm      mov   bl, al
   asm      or    al, 080h
   asm      out   061h, al       /* set bit 7 and write */
   asm      mov   al, bl
   asm      out   061h, al       /* write again, bit 7 clear */

   asm      mov   al, 020h       /* reset PIC */
   asm      out   020h, al

         /* end of re-set code */

   asm      sti

/*save the raw scan code in a 256 byte buffer*/
   *(gb_scan_q+gb_scan_tail)=gb_scan;
   ++gb_scan_tail;
}

/*

save the old int9 ISR vector, and install our own
*/
/* ---------------------- init_keyboard() ---------------- April 17,1993 */
void init_keyboard(void)
{
   BYTE far *bios_key_state;

   /* save old BIOS key board handler */
   oldkb=getvect(9);

   /*turn off num-lock via BIOS*/ 
   bios_key_state=MK_FP(0x040, 0x017);
   *bios_key_state&=(~(32 | 64));     /* toggle off caps lock and
                                        num lock bits in the BIOS variable*/
   oldkb();      /*call BIOS key handler to change keyboard lights*/

   gb_scan_head=0;
   gb_scan_tail=0;
   gb_scan=0;

   /* install our own handler */
   setvect(9, get_scan);

}

/* restore the bios keyboard handler */
/* ---------------------- deinit_keyboard() -------------- April 17,1993 */
void deinit_keyboard(void)
{
   setvect(9, oldkb);
}

int update_keystates(int keystates, int* key_map, int key_map_size) {
   BYTE key_event, was_released;
   int i;
   while ( gb_scan_head != gb_scan_tail )
      {
         key_event = gb_scan_q[gb_scan_head];

         was_released = !!(key_event & KEY_RELEASED);
         key_event = key_event & ~KEY_RELEASED;

         for(i = 0; i < key_map_size; i++) {
            if(key_map[i] == key_event) {
               if(was_released) {
                  keystates &= ~(1 << i);
               } else {
                  keystates |= (1 << i);
               }
               break;
            }
         }

         

         gb_scan_head++;
      
      }
   return keystates;
}

void get_keys_hit(char *keybuf) {
   BYTE key_event, was_released;
   int i;
   while ( gb_scan_head != gb_scan_tail )
      {
         key_event = gb_scan_q[gb_scan_head];

         was_released = !!(key_event & KEY_RELEASED);
         key_event = key_event & ~KEY_RELEASED;

         if(!was_released)
            keybuf[key_event >> 3] |= 1 << (key_event & 7);
         else {
            keybuf[key_event >> 3] &= ~(1 << (key_event & 7));
         }

         gb_scan_head++;
      
      }
}

void clear_keybuf(char *keybuf) {
   memset(keybuf, 0, 32);
}

int test_keybuf(char *keybuf, int keycode) {
   return keybuf[keycode >> 3] & 1 << (keycode & 7);
}

void disable_repeat()
{
    union REGS regs;
    regs.h.ah = 0x03;
    regs.h.al = 0x04;
    int86(0x16, &regs, &regs);
}