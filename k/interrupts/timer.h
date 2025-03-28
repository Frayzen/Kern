#ifndef TIMER_H
#define TIMER_H


void setup_timer(void);
unsigned long get_tick(void);
void timer_interrupt(void);
void wait(unsigned long tick);

#endif /* !TIMER_H */
