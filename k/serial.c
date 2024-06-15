#include "serial.h"
#include "io.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8


#define THR 0
#define RBR 0
#define DLL 0
#define DLH 1
#define IIR 2
#define LCR 3
#define LSR 5


void serial_init(void)
{
    static int init = 0;
    if (init)
        return;
    init = 1;
    static const u8 dlabMask = 0x80;
    static const u8 wordMask = 0x3; // 8 bits word length

    outb(COM1 + DLH, 0x0); // Disable interupts
    outb(COM1 + LCR, dlabMask); // Set the DLAB value
    outb(COM1 + DLH, 0x00); // Set baudrate to 9600
    outb(COM1 + DLL, 0x05);
    outb(COM1 + LCR, wordMask); // Also reset the DLAB value
}

static int received()
{
    return (inb(COM1 + LSR) & 1);
}

char read(void)
{
    serial_init();
    while(!received())
        continue;
    return inb(COM1);
}

int writechar(const char buf)
{
    serial_init();
    static const u8 emptyDataHoldingReg = 0x20;
    while (!(inb(COM1 + LSR) & emptyDataHoldingReg))
        continue;
    outb(COM1 + THR, buf);
    return 1;
}

int write(const char *buf, size_t count)
{
    serial_init();
    for (size_t i = 0; i < count; i++)
        writechar(buf[i]);
    return count;
}

// TOOL FUNCTIONS

void print_hex(unsigned int i)
{
	int i2 = i & 0xF;
	if (i2 < 10)
		printchar('0' + i2);
	else
		printchar('A' + i2 - 10);
}

// Printing functions using printchar only
void print_uint(unsigned int val, int size)
{
	/* print("0x"); */
	for (int i = size - 1; i >= 0; i--)
		print_hex(val >> (4 * i));
}

