.global isr
.extern interrupt_handler

isr:
    jmp .context_save
.interrupt_handler:
    cld    
    # C code following the sysV ABI requires DF to be clear on function entry
    call interrupt_handler
    jmp .context_restore

.context_save:
    # Save general purpose registers
    push %eax
    push %ebx
    push %ecx
    push %edx
    push %esi
    push %edi
    push %ebp
    # save segment registers
    push %ds
    push %es
    push %fs
    push %gs
    push %ss

    # save stack pointer
    mov %esp, %eax
    push %eax
    jmp .interrupt_handler

.context_restore:
    # restore stack pointer
    pop %eax
    mov %eax, %esp

    # restore segment registers
    pop %ss
    pop %gs
    pop %fs
    pop %es
    pop %ds

    # restore general purpose registers
    pop %ebp
    pop %edi
    pop %esi
    pop %edx
    pop %ecx
    pop %ebx
    pop %eax

    iret
    
