#include "keyboard.h"
#include "io.h"
#include "stdio.h"

static const char kbd_US[128] = {
	0,   27,   '1', '2',  '3',  '4',  '5',	'6', '7',
	'8', '9',  '0', '-',  '=',  '\b', '\t', /* <-- Tab */
	'q', 'w',  'e', 'r',  't',  'y',  'u',	'i', 'o',
	'p', '[',  ']', '\n', 0, /* <-- control key */
	'a', 's',  'd', 'f',  'g',  'h',  'j',	'k', 'l',
	';', '\'', '`', 0,    '\\', 'z',  'x',	'c', 'v',
	'b', 'n',  'm', ',',  '.',  '/',  0,	'*', 0, /* Alt */
	' ', /* Space bar */
	0, /* Caps lock */
	0, /* 59 - F1 key ... > */
	0,   0,	   0,	0,    0,    0,	  0,	0,   0, /* < ... F10 */
	0, /* 69 - Num lock*/
	0, /* Scroll Lock */
	0, /* Home key */
	0, /* Up Arrow */
	0, /* Page Up */
	'-', 0, /* Left Arrow */
	0,   0, /* Right Arrow */
	'+', 0, /* 79 - End key*/
	0, /* Down Arrow */
	0, /* Page Down */
	0, /* Insert Key */
	0, /* Delete Key */
	0,   0,	   0,	0, /* F11 Key */
	0, /* F12 Key */
	0, /* All other keys are undefined */
};
#define IO_KEYBOARD 0x60
#define KEYBOARD_BUFFER_SIZE 128
static int keyboard_buffer[KEYBOARD_BUFFER_SIZE] = {0};
static int keyboard_buffer_pos = 0;

void handle_keyboard(void)
{
    int k = inb(IO_KEYBOARD);
    if (k & 0x80)
        return;
    if (keyboard_buffer_pos == KEYBOARD_BUFFER_SIZE)
        return;
    keyboard_buffer[keyboard_buffer_pos] = k;
    keyboard_buffer_pos++;
    printf("Register %c\n", kbd_US[k]);
}

char get_last_key(void)
{
    if (keyboard_buffer_pos == 0)
        return -1;
    int ret = keyboard_buffer[--keyboard_buffer_pos];
    printf("Consume %c\n", kbd_US[ret]);
    return ret;
}


