#include "handler.h"
#include "interrupts/ints.h"
#include "serial.h"

void interrupt_handler(stack *s)
{
	switch (s->int_no) {
	case 0:
		println("Divide error");
		break;
	case 14:
		println("Page fault");
		break;
	case 64:
		/* println("System clock"); */
		break;
	case 65:
		println("Keyboard");
		break;
	default:
		print("Unknown interrupt (");
		print_uint(s->int_no, 4);
		println(")");
		asm volatile("hlt");
		break;
	}
	send_eoi(10);
}
