#include "io.h"
#include "serial.h"

#define MASTER_PIC_A 0x20
#define MASTER_PIC_B 0x21
#define SLAVE_PIC_A 0xA0
#define SLAVE_PIC_B 0xA1
#define ICW1_ICW4 0x01 /* Indicates that ICW4 will be present */
#define ICW1_INIT 0x10 /* Initialization - required! */

void pic_setup(void)
{
	println("Setting up PIC...");

	// ICW1
	outb(MASTER_PIC_A, ICW1_INIT | ICW1_ICW4);
	outb(SLAVE_PIC_A, ICW1_INIT | ICW1_ICW4);

	// ICW2
	outb(MASTER_PIC_B, IRQ_MASTER_OFFSET);
	outb(SLAVE_PIC_B, IRQ_SLAVE_OFFSET);

	// ICW3
	outb(MASTER_PIC_B, 0x04); // Set the second pin as a slave
	outb(SLAVE_PIC_B, 0x02); // Set the pin of the slave as IRQ2

	// ICW4
	// When no special mode of operation is required, the programmer can just clear every configuration bit
	outb(MASTER_PIC_B, 0x01);
	outb(SLAVE_PIC_B, 0x01);

	// Unmask all interrupts
	outb(MASTER_PIC_B, 0x0);
	outb(SLAVE_PIC_B, 0x0);

	println("PIC set up");
}

#define PIC_EOI 0x20 /* End-of-interrupt command code */
void pic_send_eoi(unsigned int irq)
{
	(void)irq;
	if (irq >= IRQ_SLAVE_OFFSET)
		outb(0xA0, PIC_EOI);
	outb(MASTER_PIC_A, PIC_EOI);
}
