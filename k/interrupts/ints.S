#include "isr_list.h"

.extern interrupt_handler
retval: .long 0
isr_common:
    pusha
    mov %ds, %eax
    push %eax
    mov %cr2, %eax
    push %eax

    mov $0x10, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs

    push %esp
    call interrupt_handler
    mov %eax, retval

    add $8, %esp
    pop %ebx
    mov %ebx, %ds
    mov %ebx, %es
    mov %ebx, %fs
    mov %ebx, %gs
    popa
    add $8, %esp
    sti
    mov retval, %eax
    iret

#define X(id, name, errcode)\
.global isr##id;\
isr##id:;\
    cli;\
    .if errcode == 0;\
    push $0;\
    .endif;\
    push $id;\
    jmp isr_common;

ISR_LIST
IRQ_LIST
#undef X
