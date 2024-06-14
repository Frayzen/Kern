.global gdtFlush
gdtFlush:
    # Set CR0
    # movl $0x10, %eax
    # movl %eax, %cr0
    # Likewise, setting data in segment register
    # First segment = 0x08
    # Second segment = 0x10
    movw $0x08, %ax 
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ret
