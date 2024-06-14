.global gdtFlush
gdtFlush:
    # Set CR0
    # movl $0x10, %eax
    # movl %eax, %cr0
    # Likewise, setting data in segment register
    # First segment = 0x08
    # Second segment = 0x10
    movw $0x10, %ax 
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    pushl $0x08 /* push %cs on the stack */
    pushl $1f /* push %eip on the stack */
    lret /* far return */
1:       /* After the lret you will get here, with cs set to 0x42 */
    ret
