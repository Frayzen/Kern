#include "handler.h"
#include "interrupts/ints.h"
#include "interrupts/keyboard.h"
#include "k/kstd.h"
#include "serial.h"

unsigned int syscall_handler(stack *s)
{
    println("Syscall");
    switch (s->eax) {
        case SYSCALL_GETKEY:
            return get_last_key();
        default:
            return -1;
    }
}

unsigned int interrupt_handler(stack *s)
{
	switch (s->int_no) {

	case 0:
		println("Divide error");
		break;
	case 14:
		println("Page fault");
		break;
	case 64:
		println("System clock");
		break;
	case 65:
        handle_keyboard();
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
    if (s->int_no >= IRQ_MASTER_OFFSET && s->int_no <= IRQ_SLAVE_OFFSET)
	    send_eoi(s->int_no);
    return 0;
}
