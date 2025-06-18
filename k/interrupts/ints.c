#include "ints.h"
#include "drivers/apic/apic.h"
#include "drivers/pic/pic.h"
#include "drivers/config.h"
#include "drivers/pit/pit.h"
#include "k/types.h"
#include "serial.h"
#include "isr_list.h"
#include <stdio.h>

#define MAX_ISR_NB 256

void setup_idt(void)
{
	pic_setup();
	pit_setup();
	if (USE_APIC)
		apic_setup();
	println("Setting up IDT...");
	static struct gate_descriptor gates[MAX_ISR_NB];
#define X(id, key, name, errcode)                               \
	gates[id].offset_low = OFFSET_LOW(isr##key);            \
	gates[id].segment_selector = 0x8;                       \
	gates[id].offset_high = OFFSET_HIGH(isr##key);          \
	gates[id].type = INT_GATE_32B, gates[id].privilege = 0; \
	gates[id].present = 1;
	ISR_LIST IRQ_LIST
#undef X
		struct idt_descriptor idt_holder = { .limit = sizeof(gates) - 1,
						     .base = (uint_ptr)&gates };
	asm volatile("lidt %0"
		     : /* no output */
		     : "m"(idt_holder)
		     : "memory");
	printf("IDT loaded at %x\n", idt_holder.base);
}
