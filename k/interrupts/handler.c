#include "handler.h"
#include "interrupts/ints.h"
#include "interrupts/keyboard.h"
#include "interrupts/timer.h"
#include "k/kstd.h"
#include "serial.h"

unsigned int syscall_handler(stack *s)
{
	switch (s->eax) {
	case SYSCALL_GETKEY:
		return get_last_key();
	case SYSCALL_GETTICK:
		return get_tick();
	default:
		return -1;
	}
}

void handle_irq(unsigned int irq)
{
	switch (irq) {
	case IRQ0:
		timer_interrupt();
		break;
	case IRQ1:
		handle_keyboard();
		break;
	default:
		print("Unhandled IRQ");
		print_uint(irq - IRQ_MASTER_OFFSET, 4);
        println();
		asm volatile("hlt");
		break;
	}
	send_eoi(irq);
}

unsigned int interrupt_handler(stack *s)
{
	if (s->int_no >= IRQ_MASTER_OFFSET && s->int_no <= IRQ_SLAVE_OFFSET) {
		handle_irq(s->int_no);
		return 0;
	}
	switch (s->int_no) {
	case 0:
		println("Divide error");
		break;
	case 14:
		println("Page fault");
		break;
	case 128: /* Custom Syscall */
		return syscall_handler(s);
	default:
		print("Unknown interrupt (");
		print_uint(s->int_no, 4);
		println(")");
		asm volatile("hlt");
		break;
	}
	return 0;
}
