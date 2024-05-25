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
    outb(COM1 + IIR, 0x0); // Disable interupts

    // Set the DLAB value
    static const u8 dlabMask = 0x80;
    outb(COM1 + LCR, dlabMask);

    // Set baudrate to 9600
    outb(COM1 + DLH, 0x00);
    outb(COM1 + DLL, 0x0C);

    static const u8 wordMask = 0x3; // 8 bits word length
    outb(COM1 + LCR, wordMask); // Also reset the DLAB value
}

char read(void)
{
    static const u8 receivedMask = 0x1;
    while (!(inb(COM1 + RBR) & receivedMask))
        continue;
    return inb(COM1 + RBR);
}

int write(const char *buf, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        static const u8 emptyDataHoldingReg = 0x20;
        while (!(inb(COM1 + LSR) & emptyDataHoldingReg))
            continue;
        outb(COM1 + THR, buf[i]); 
    }
    return count;
}
