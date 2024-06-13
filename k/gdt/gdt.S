.global gdtFlush
gdtFlush:
    # Set CR0
    movl $0x42, %eax
    movl %eax, %cr0
    # Likewise, setting data in segment register
    movw $0x10, %ax
    mov  %ax, %ds
    mov  %ax, %es
    mov  %ax, %fs
    mov  %ax, %gs
    mov  %ax, %ss
    mov  %ax, %cs
    ret
