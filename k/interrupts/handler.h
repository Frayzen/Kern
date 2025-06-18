#ifndef HANDLER_H
#define HANDLER_H

#include "k/compiler.h"
#include "isr_list.h"
#include "k/types.h"

enum interrupts {
#define X(id, key, name, errcode) IRQ_##key = id,
	IRQ_LIST
#undef X
#define X(id, key, name, errcode) ISR_##key = id,
		ISR_LIST
#undef X
};

struct stack {
	u32 cr2, ds, edi, esi;
	u32 ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, csm, eflags, useresp, ss;
} __packed;

void print_stack(struct stack* s);
void print_backtrace(struct stack* s);
void print_selector_errcode(u32 err_code);
unsigned int interrupt_handler(struct stack *s);
void print_idt(void);

extern int int_count;

#endif /* !HANDLER_H */
