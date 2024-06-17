#include "timer.h"
#include "io.h"
#include "serial.h"

#define PIT_REG_COUNTER_0 0x40
#define PIT_REG_COUNTER_1 0x41
#define PIT_REG_COUNTER_2 0x42
#define PIT_REG_CONTROL 0x43

#define INTERNAL_FREQUENCY 1193180
#define DESIRED_FREQUENCY 1000
#define RELOAD_VALUE (INTERNAL_FREQUENCY / 1000)

unsigned int get_frequency(void)
{
	// al = channel in bits 6 and 7, remaining bits clear
	outb(PIT_REG_CONTROL, 0x0);
	unsigned int count = 0;
	count = inb(PIT_REG_COUNTER_0); // Low byte
	count |= inb(PIT_REG_COUNTER_0) << 8; // High byte
	return count;
}

void set_pit_count(unsigned count)
{
	// Disable interrupts
	asm volatile("cli");

	// Set low byte
	outb(PIT_REG_COUNTER_0, count & 0xFF); // Low byte
	outb(PIT_REG_COUNTER_0, (count & 0xFF00) >> 8); // High byte
	return;
}

void setup_timer(void)
{
	print_uint(get_frequency(), 2);
	println();

	/*
     * Bits         Usage
     * 6 and 7      Select channel :
     *                 0 0 = Channel 0
     *                 0 1 = Channel 1
     *                 1 0 = Channel 2
     *                 1 1 = Read-back command (8254 only)
     * 4 and 5      Access mode :
     *                 0 0 = Latch count value command
     *                 0 1 = Access mode: lobyte only
     *                 1 0 = Access mode: hibyte only
     *                 1 1 = Access mode: lobyte/hibyte
     * 1 to 3       Operating mode :
     *                 0 0 0 = Mode 0 (interrupt on terminal count)
     *                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
     *                 0 1 0 = Mode 2 (rate generator)
     *                 0 1 1 = Mode 3 (square wave generator)
     *                 1 0 0 = Mode 4 (software triggered strobe)
     *                 1 0 1 = Mode 5 (hardware triggered strobe)
     *                 1 1 0 = Mode 2 (rate generator, same as 010b)
     *                 1 1 1 = Mode 3 (square wave generator, same as 011b)
     * 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
     */
	outb(PIT_REG_CONTROL, 0b110000);
	set_pit_count(RELOAD_VALUE);
	print_uint(get_frequency(), 2);
	println();
	println("OK");
}
