.extern interrupt_handler
.macro isr arg
    .global isr\arg
    isr\arg:
        cli
        push $0
        push $\arg
        jmp isr_common
.endm

.macro isr_errcode arg
    .global isr\arg
    isr\arg:
        cli
        push $\arg
        jmp isr_common
.endm

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


.global isr_default
isr_default:
isr 230

isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
isr_errcode 8
isr 9
isr_errcode 10
isr_errcode 11
isr_errcode 12
isr_errcode 13
isr_errcode 14
isr 15
isr 16
isr 17
isr 18
isr 19
isr 20
isr 21
isr 22
isr 23
isr 24
isr 25
isr 26
isr 27
isr 28
isr 29
isr 30
isr 31

isr 128;

# IRQs
isr 64
isr 65
