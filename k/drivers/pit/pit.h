#ifndef PIT_H
#define PIT_H

void pit_setup(void);
unsigned long get_tick(void);
void pit_interrupt(void);
void wait(unsigned long tick);

#endif /* !PIT_H */
