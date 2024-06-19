#ifndef HANDLER_H
#define HANDLER_H

#define IRQ0 64
#define IRQ1 65
#define IRQ2 66
#define IRQ3 67
#define IRQ4 68
#define IRQ5 69
#define IRQ6 70
#define IRQ7 71
#define IRQ8 72
#define IRQ9 73
#define IRQ10 74
#define IRQ11 75
#define IRQ12 76
#define IRQ13 77
#define IRQ14 78
#define IRQ15 79

typedef struct {
	unsigned int cr2, ds, edi, esi : 32;
	unsigned int ebp, esp, ebx, edx, ecx, eax : 32;
	unsigned int int_no, err_code : 32;
	unsigned int eip, csm, eflags, useresp, ss : 32;
} __attribute__((packed)) stack;

unsigned int interrupt_handler(stack *s);

#endif /* !HANDLER_H */
