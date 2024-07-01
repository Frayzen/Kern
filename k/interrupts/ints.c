#include "ints.h"
#include "interrupts/timer.h"
#include "k/compiler.h"
#include "serial.h"
#include "isr_list.h"

typedef struct {
	unsigned int offset_low : 16;
	unsigned int selector : 16;
	unsigned int __unused : 8;
	unsigned int type : 5;
	unsigned int privilege : 2; // (ring of privilege)
	unsigned int present : 1;
	unsigned int offset_high : 16;
} __packed gate_descriptor;

typedef struct {
	unsigned int limit : 16;
	unsigned int base : 32;
} __packed idt_descriptor;

#define X(id, name, errcode) extern void isr##id(void);
ISR_LIST
#undef X

#define FN_PTR(Fn) (unsigned int)((void (*)(void))(Fn))
#define OFFSET_LOW(Fn) (0xFFFF & (FN_PTR(Fn)))
#define OFFSET_HIGH(Fn) ((0xFFFF0000 & (FN_PTR(Fn))) >> 16)

void setup_idt(void)
{
	setup_pic();
	setup_timer();
	println("Setting up IDT...");
	gate_descriptor gates[] = {
#define X(id, name, errcode)                         \
	[id] = {                                     \
		.offset_low = OFFSET_LOW(isr##id),   \
		.selector = 0x8,                     \
		.offset_high = OFFSET_HIGH(isr##id), \
		.type = GATE_TYPE_INT,               \
		.privilege = 0,                      \
		.present = 1,                        \
	},
		ISR_LIST
#undef X
	};
	println();
	print_uint(sizeof(gates), 4);
	idt_descriptor idt_holder = {
		.limit = sizeof(gates) - 1,
		.base = (unsigned int)&gates
	};
	asm volatile("lidt %0"
		     : /* no output */
		     : "m"(idt_holder)
		     : "memory");
	println("IDT loaded");
}
