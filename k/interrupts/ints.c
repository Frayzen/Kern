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

static idt_descriptor idt_holder;

void print_gate(gate_descriptor gate_desc)
{
	print("Offset: ");
	print_uint(gate_desc.offset_high, 4);
	print_uint(gate_desc.offset_low, 4);
	print("  Selector: ");
	print_uint(gate_desc.selector, 4);
	print("  Type: ");
	print_uint(gate_desc.type, 1);
	print("  Privilege: ");
	print_uint(gate_desc.privilege, 2);
	print("  Present: ");
	print_uint(gate_desc.present, 1);
	println("");
}

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
	print_gate(gates[0]);
	println();
	print_uint(sizeof(gates), 4);
	idt_holder.limit = sizeof(gates) - 1;
	idt_holder.base = (unsigned int)&gates;
	asm volatile("lidt %0"
		     : /* no output */
		     : "m"(idt_holder)
		     : "memory");
	println("IDT loaded");
}
