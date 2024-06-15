#include "handler.h"
#include "serial.h"

void interrupt_handler(stack* stack)
{
    println("Interrupt handler called");
    print_uint(stack->eflags, 4);
    print_uint(stack->err_code, 2);
    print_uint(stack->int_no, 1);
    println();
}
