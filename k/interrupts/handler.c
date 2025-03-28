#include "handler.h"
#include "isr_list.h"
#include "interrupts/ints.h"
#include "interrupts/keyboard.h"
#include "interrupts/timer.h"
#include "k/kstd.h"
#include "serial.h"
#include "stdio.h"

int int_count = 0;

unsigned int syscall_handler(struct stack *s)
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
	case IRQ_SYSTEM_CLOCK:
		timer_interrupt();
		break;
	case IRQ_KEYBOARD:
		handle_keyboard();
		break;
	case IRQ_IDE_CONTROLLER:
	case IRQ_FLOPPY_DISK_CONTROLLER:
		printf("Disk IRQ received\n");
		break;
  case IRQ_GENERIC_HARD_DISK:
		printf("======================\n");
		printf("General hard disk IRQ\n");
		printf("======================\n");
    return;
	default:
		print("Unhandled IRQ");
		printf("%d", irq - IRQ_MASTER_OFFSET);
		println();
		asm volatile("hlt");
		break;
	}
	send_eoi(irq);
}

unsigned int interrupt_handler(struct stack *s)
{
  int_count++;
	if (s->int_no >= IRQ_MASTER_OFFSET &&
	    s->int_no <= IRQ_MASTER_OFFSET + IRQ_LIMIT) {
		handle_irq(s->int_no);
		return 0;
	}
	if (s->int_no == ISR_CUSTOM_SYSCALL) {
		/* Custom Syscall */
		println("Custom Syscall");
		return syscall_handler(s);
	}
	if (s->int_no == ISR_GENERAL_PROTECTION_FAULT) {
		printf("======== KERNEL PANIC ========\n", s->int_no);
		printf("General protection fault ! (on 0x%x)\n");
    print_stack(s);
		printf("System halted.\n");
		printf("======== KERNEL PANIC ========\n", s->int_no);
		while (1)
			continue;
	}

	switch (s->int_no) {
#define X(id, key, name, errcode)                                        \
	case id:                                                         \
		printf("======== HANDLE INT 0x%x========\n", s->int_no); \
		println(name);                                           \
		printf("======== HANDLE INT 0x%x========\n", s->int_no); \
		break;
		ISR_LIST
#undef X
	default:
		print("Unknown interrupt (");
		printf("%d", s->int_no);
		println(")");
		asm volatile("hlt");
		break;
	}
	return 0;
}
