#ifndef INTS_H
#define INTS_H

#include "isr_list.h"

void setup_idt(void);
void pic_setup(void);
void pic_send_eoi(unsigned int irq);

#define X(id, key, name, errcode) extern void isr##key(void);
ISR_LIST
IRQ_LIST
#undef X

#define FN_PTR(Fn) (unsigned int)((void (*)(void))(Fn))
#define OFFSET_LOW(Fn) (0xFFFF & (FN_PTR(Fn)))
#define OFFSET_HIGH(Fn) ((0xFFFF0000 & (FN_PTR(Fn))) >> 16)


#endif /* !INTS_H */
