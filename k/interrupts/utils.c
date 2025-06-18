#include "interrupts/handler.h"
#include "interrupts/ints.h"
#include <stdint.h>
#include <stdio.h>

void print_stack(struct stack *s)
{
	printf("           == STACK ==\n");
	printf("cr2: 0x%.8x |  ds: 0x%.8x\n", s->cr2, s->ds);
	printf("edi: 0x%.8x | esi: 0x%.8x\n", s->edi, s->esi);
	printf("ebp: 0x%.8x | esp: 0x%.8x\n", s->ebp, s->esp);
	printf("ebx: 0x%.8x | edx: 0x%.8x\n", s->ebx, s->edx);
	printf("ecx: 0x%.8x | eax: 0x%.8x\n", s->ecx, s->eax);
	printf("int: 0x%.8x | err: 0x%.8x\n", s->int_no, s->err_code);
	printf("eip: 0x%.8x | csm: 0x%.8x\n", s->eip, s->csm);
	printf("efg: 0x%.8x | usr: 0x%.8x\n", s->eflags, s->useresp);
	printf("ss: 0x%.8x\n", s->ss);
	printf("ERROR_CODE: 0x%.8x\n", s->err_code);
}

void print_selector_errcode(u32 err_code)
{
	printf(" |  External: %d\n", err_code & 0b1);
	char tbl_letter[] = { 'G', 'I', 'L', 'I' };
	printf(" |  Table: %cDT\n", tbl_letter[(err_code >> 1) & 0b11]);
	u32 id = (err_code >> 3) & 0b1111111111111;
	printf(" |  Index: 0x%.1x (%d)\n", id, id);
}

void print_backtrace(struct stack *s)
{
	// Start with the current EBP from the stack frame
	uint32_t *ebp = (uint32_t *)s->ebp;

	printf("\nBacktrace:\n");

	// Limit the number of frames we'll trace to avoid infinite loops
	for (int i = 0; i < 20 && ebp != 0; i++) {
		// The return address is at ebp+1
		uint32_t eip = ebp[1];

		printf("  #%d [0x%08x]\n", i, eip);

		// Move to the next frame (previous ebp is at ebp[0])
		uint32_t *next_ebp = (uint32_t *)ebp[0];

		// Basic sanity check - the stack should grow downward
		if (next_ebp <= ebp) {
			break;
		}

		ebp = next_ebp;
	}

	printf("End of backtrace.\n");
}

void print_idt(void)
{
	struct idt_descriptor idt_holder = {};

	// Disable interrupts (to safely read IDTR)
	asm volatile("cli");
	asm volatile("sidt %0" : "=m"(idt_holder));
	asm volatile("sti");

	struct gate_descriptor *desc =
		(struct gate_descriptor *)idt_holder.base;

	printf("IDT Base: 0x%lx, Limit: 0x%x\n", idt_holder.base,
	       idt_holder.limit);

	for (u32 i = 0;
	     i < (idt_holder.limit + 1) / sizeof(struct gate_descriptor); i++) {
		unsigned long handler_addr =
			((unsigned long)desc[i].offset_high << 16) |
			desc[i].offset_low;

		char *type_str[] = {
			[TASK_GATE] = "TASK_GATE",
			[INT_GATE_16B] = "INT_GATE_16B",
			[TRAP_GATE_16B] = "TRAP_GATE_16B",
			[INT_GATE_32B] = "INT_GATE_32B",
			[TRAP_GATE_32B] = "TRAP_GATE_32B",
		};
		printf("INT %3d: Handler=0x%08lx, Segment=0x%04x, Type=%s Present=%d, Privilege=%d)\n",
		       i, handler_addr, desc[i].segment_selector,
		       type_str[desc[i].type], desc[i].present,
		       desc[i].privilege);
	}
}
