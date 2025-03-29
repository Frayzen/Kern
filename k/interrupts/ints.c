#include "ints.h"
#include "interrupts/timer.h"
#include "k/compiler.h"
#include "serial.h"
#include "isr_list.h"

struct gate_descriptor {
	unsigned int offset_low : 16;
	unsigned int selector : 16;
	unsigned int __unused : 8;
	unsigned int type : 5;
	unsigned int privilege : 2; // (ring of privilege)
	unsigned int present : 1;
	unsigned int offset_high : 16;
} __packed;

struct idt_descriptor {
	unsigned int limit : 16;
	unsigned int base : 32;
} __packed;

void setup_idt(void)
{
	pic_setup();
	setup_timer();
	println("Setting up IDT...");
	struct gate_descriptor gates[] = {
#define X(id, key, name, errcode)                         \
	[id] = {                                     \
		.offset_low = OFFSET_LOW(isr##key),   \
		.selector = 0x8,                     \
		.offset_high = OFFSET_HIGH(isr##key), \
		.type = GATE_TYPE_INT,               \
		.privilege = 0,                      \
		.present = 1,                        \
	},
		ISR_LIST IRQ_LIST
#undef X
	};
	struct idt_descriptor idt_holder = { .limit = sizeof(gates) - 1,
					     .base = (unsigned int)&gates };
	asm volatile("lidt %0"
		     : /* no output */
		     : "m"(idt_holder)
		     : "memory");
	println("IDT loaded");
}
