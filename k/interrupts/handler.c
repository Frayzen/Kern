#include "handler.h"
#include "drivers/apic/apic.h"
#include "drivers/config.h"
#include "drivers/disk/nvme/nvme_admin.h"
#include "drivers/disk/nvme/nvme_io.h"
#include "isr_list.h"
#include "interrupts/ints.h"
#include "interrupts/keyboard.h"
#include "drivers/pit/pit.h"
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
		pit_interrupt();
		break;
	case IRQ_KEYBOARD:
		handle_keyboard();
		break;
	case IRQ_NVME_ADMIN_QUEUE:
		nvme_process_admin_cq();
		break;
	case IRQ_NVME_IO_QUEUE:
		nvme_process_io_cq();
		break;
		// Unhandled yet
	case IRQ_PRIM_IDE_CONT:
	case IRQ_SEC_IDE_CONT:
		break;
	default:
		print("Unhandled IRQ");
		printf("%d (ISR %d)", irq - IRQ_MASTER_OFFSET, irq);
		println();
		asm volatile("hlt");
		break;
	}
	if (USE_APIC)
		apic_send_eoi();
	else
		pic_send_eoi(irq);
}

unsigned int interrupt_handler(struct stack *s)
{
	int_count++;
	if (s->int_no >= IRQ_MASTER_OFFSET &&
	    s->int_no <= IRQ_MASTER_OFFSET + IRQ_LIMIT) {
		handle_irq(s->int_no);
		return 0;
	} else if (s->int_no == ISR_GENERAL_PROTECTION_FAULT) {
		printf("======== KERNEL PANIC ========\n", s->int_no);
		printf("General protection fault ! (on 0x%x)\n");
		print_stack(s);
		print_selector_errcode(s->err_code);
		print_backtrace(s);
		print_idt();
		printf("System halted.\n");
		printf("======== KERNEL PANIC ========\n", s->int_no);
		// if (s->cr2 == 0) {
		// 	printf("Panic probably due to NULL pointer dereference\n");
		// 	printf("Please check in gdb: ('list *0x%x')", s->eip);
		// }
		while (1)
			continue;
	} else if (s->int_no == ISR_SEGMENT_NOT_PRESENT) {
		printf("======== KERNEL PANIC ========\n", s->int_no);
		printf("Segment not present\n");
		print_stack(s);
		print_selector_errcode(s->err_code);
		print_backtrace(s);
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
