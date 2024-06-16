#ifndef HANDLER_H
#define HANDLER_H

typedef struct {
    unsigned int vector_id, esp, ss, gs, fs, es, ds, ebp, edi, esi, edx, ecx, ebx, eax : 32;
    unsigned int errcode, eip : 32;
    unsigned int cs : 16;
    unsigned int eflags : 32;
} __attribute__((packed)) stack;

void interrupt_handler(stack *s);

#endif /* !HANDLER_H */
