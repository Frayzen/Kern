.global context_save
.global context_restore

.extern gdt_holder

context_save:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    push %ss
    mov %esp, %eax
    push %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    mov %eax, %ss
    pop %eax
    mov %eax, %esp
    pop %eax
    pop %ss
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    ret
