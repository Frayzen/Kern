#ifndef PIC_H
#define PIC_H

void pic_send_eoi(unsigned int irq);
void pic_setup(void);

#endif /* !PIC_H */
