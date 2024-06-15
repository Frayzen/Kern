#ifndef HANDLER_H
#define HANDLER_H

typedef struct  {
    unsigned int ss, gs, fs, es, ds : 16;
    unsigned int ebp, edi, esi, edx, ecx, ebx, eax : 32;
    unsigned int int_no : 8;
    unsigned int err_code : 8;
    unsigned int eip : 16;
    unsigned int cs : 16;
    unsigned int eflags : 16;
    unsigned int useresp : 16;
} __attribute__((packed)) stack;


void interrupt_handler(stack* stack);

#endif /* !HANDLER_H */
