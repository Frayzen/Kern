#include "interrupts/handler.h"
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
	printf(" ss: 0x%.8x\n", s->ss);
	printf("           == STACK ==\n");
}
