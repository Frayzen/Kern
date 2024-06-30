#ifndef TSS_H
#define TSS_H

#include "k/compiler.h"
typedef struct {
    unsigned int prev_task : 32;
    unsigned int esp0 : 32;
    unsigned int ss0 : 32;
    unsigned int esp1 : 32;
    unsigned int ss1 : 32;
    unsigned int esp2 : 32;
    unsigned int ss2 : 32;
    unsigned int cr3 : 32;
    unsigned int eip : 32;
    unsigned int eflags : 32;
    unsigned int eax : 32;
    unsigned int ecx : 32;
    unsigned int edx : 32;
    unsigned int ebx : 32;
    unsigned int esp : 32;
    unsigned int ebp : 32;
    unsigned int esi : 32;
    unsigned int edi : 32;
    unsigned int es : 32;
    unsigned int cs : 32;
    unsigned int ss : 32;
    unsigned int ds : 32;
    unsigned int fs : 32;
    unsigned int gs : 32;
    unsigned int ldt : 32;
    unsigned int trapno : 32;
    unsigned int error_code : 32;
    unsigned int eip_err : 32;
    unsigned int cs_err : 32;
    unsigned int ss_err : 32;
    unsigned int ds_err : 32;
    unsigned int fs_err : 32;
    unsigned int gs_err : 32;
    unsigned int ldt_err : 32;
    unsigned int iomap_base : 32;
} __packed tss_entry;



tss_entry* setup_tss();


#endif /* !TSS_H */
