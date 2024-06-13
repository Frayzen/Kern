.global gdtFlush
gdtFlush:
    # Set CR0
    movl $0x42, %eax
    movl %eax, %cr0
    # Likewise, setting data in segment register
    movw $0x42, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ret
