#ifndef INTS_H
#define INTS_H

#include "isr_list.h"

void setup_idt(void);
void setup_pic(void);
void send_eoi(unsigned int irq);

#endif /* !INTS_H */
