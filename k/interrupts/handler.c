#include "handler.h"
#include "serial.h"

void interrupt_handler(stack *stack)
{
	println("Interrupt handler called");
	print("Stack:");
	print_uint(stack->int_no, 2);
	print(" Error code:");
	print_uint(stack->err_code, 2);
	print(" EFLAGS:");
	print_uint(stack->eflags, 4);
	print(" EIP:");
	print_uint(stack->eip, 4);
	print(" CS:");
	print_uint(stack->cs, 4);
	print(" EFLAGS:");
	print_uint(stack->eflags, 4);
	print(" EBP:");
	print_uint(stack->ebp, 8);
    println();
}
