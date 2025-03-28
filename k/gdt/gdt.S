.global gdtFlush

.equ KERN_CODE_SEG, 0x08
.equ KERN_DATA_SEG, 0x10
gdtFlush:
    # Set CR0
    # movl $0x10, %eax
    # movl %eax, %cr0
    movw $KERN_DATA_SEG, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    pushl $KERN_CODE_SEG /* push the new segment selector */
    pushl $1f /* push %eip on the stack */
    lret /* far return */
1:       /* After the lret you will get here, with cs set to 0x8 */
    ret
