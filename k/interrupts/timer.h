#ifndef TIMER_H
#define TIMER_H

void setup_timer(void);
unsigned long get_tick(void);
void timer_interrupt(void);

#endif /* !TIMER_H */
