#include "keyboard.h"
#include "io.h"

#define IO_KEYBOARD 0x60
#define KEYBOARD_BUFFER_SIZE 128
static int keyboard_buffer[KEYBOARD_BUFFER_SIZE] = {0};
static int keyboard_buffer_pos = 0;
static int keyboard_buffer_used = 0;
#define KEY_RELEASE_MASK 0x80

void handle_keyboard(void)
{
    int k = inb(IO_KEYBOARD);
    if (k & KEY_RELEASE_MASK)
        return;
    if (keyboard_buffer_used == KEYBOARD_BUFFER_SIZE)
        return;
    keyboard_buffer[keyboard_buffer_pos + keyboard_buffer_used++] = k;
}

char get_last_key(void)
{
    if (keyboard_buffer_used == 0)
        return -1;
    int ret = keyboard_buffer[keyboard_buffer_pos++];
    keyboard_buffer_pos %= KEYBOARD_BUFFER_SIZE;
    keyboard_buffer_used--;
    return ret;
}
