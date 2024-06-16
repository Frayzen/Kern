#include "handler.h"
#include "interrupts/ints.h"
#include "serial.h"

void interrupt_handler(stack *s)
{
	println("Interrupt handler called: ");
    print_uint(s->vector_id, 4);
    print(" ");
    print_uint(s->eflags, 4);
    println();
    send_eoi(10);
}
