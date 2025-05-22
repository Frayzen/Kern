#include "config.h"
#include "assert.h"
#include "drivers/apic/madt.h"
#include "drivers/msr/msr.h"
#include "drivers/rsdp/rsdp.h"
#include "drivers/rsdp/sdt.h"
#include "interrupts/handler.h"
#include <stdio.h>

#define IOAPIC_VER_REG 0x1
#define IOAPIC_RED_TBL_REGA(n) \
	(0x10 + (2 * n)) // redirection table register (lower 32 bits)
#define IOAPIC_RED_TBL_REGB(n) \
	(0x11 + (2 * n)) // redirection table register (upper 32 bits)

static volatile u32 *io_apic_base = 0;

#define IOSEL (io_apic_base)
#define IOWIN (io_apic_base + 4)

/*
 * The IO APIC uses two registers for most of its operation
 * - an address register at IOAPICBASE+0 
 * - a data register at IOAPICBASE+0x10.
 * All accesses must be done on 4 byte boundaries.
 */

static void write_reg(const u8 offset, const u32 val)
{
	/* tell IOREGSEL where we want to write to */
	*IOSEL = offset;
	/* write the value to IOWIN */
	*IOWIN = val;
}

static u32 read_reg(const u8 offset)
{
	/* tell IOREGSEL where we want to read from */
  *IOSEL = offset;
  /* read the value from IOWIN */
  return *IOWIN;
}

static void redirect(const u8 irq, const u64 vector)
{
	write_reg(IOAPIC_RED_TBL_REGA(irq), vector & 0xffffffff);
	write_reg(IOAPIC_RED_TBL_REGB(irq), vector << 32);
}

void io_apic_setup(struct MADT *madt)
{
	struct madt_entry *io_apic_entry = madt_find_entry(madt, IO_APIC);
	struct MADT_io_apic_data *io_apic_data = (void *)io_apic_entry->data;
	io_apic_base = (u32 *)(io_apic_data->address << 16);
	printf("APIC IO BASE %x\n", io_apic_base);
	printf("VERSION IS %x\n", read_reg(IOAPIC_VER_REG) & 0xFF);

	redirect(4, 0x31);

	printf("IO APIC BASE %x\n", io_apic_base);
}
