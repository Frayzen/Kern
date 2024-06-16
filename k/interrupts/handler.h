#ifndef HANDLER_H
#define HANDLER_H

typedef struct {
	unsigned int cr2, ds, edi, esi : 32;
	unsigned int ebp, esp, ebx, edx, ecx, eax : 32;
	unsigned int int_no, err_code : 32;
	unsigned int eip, csm, eflags, useresp, ss : 32;
} __attribute__((packed)) stack;

unsigned int interrupt_handler(stack *s);

#endif /* !HANDLER_H */
