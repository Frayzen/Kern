#include "io_apic.h"
#include "assert.h"
#include "drivers/apic/madt.h"
#include "drivers/config.h"
#include "drivers/msr/msr.h"
#include "drivers/rsdp/rsdp.h"
#include "drivers/rsdp/sdt.h"
#include "interrupts/handler.h"
#include "k/types.h"
#include "panic.h"
#include <stdio.h>

#define MAX_HANDLED_GSI 24
#define KEYBOARD_GSI 1

static u8 io_apic_amount = 0; // the amount of io_apic_list
static struct madt_io_apic_data io_apic_list[MAX_IO_APIC_HANDLED] = {};

#define IOAPIC_ID_REG 0x0
#define IOAPIC_VER_REG 0x1
#define IOAPIC_RED_TBL_REGA(n) \
	(0x10 + (2 * n)) // redirection table register (lower 32 bits)
#define IOAPIC_RED_TBL_REGB(n) \
	(0x11 + (2 * n)) // redirection table register (upper 32 bits)

#define BASE_ADDR(Id) ((volatile u32 *)io_apic_list[Id].address)

#define IOSEL(Id) (BASE_ADDR(Id))
#define IOWIN(Id) (BASE_ADDR(Id) + 4)

/*
 * The IO APIC uses two registers for most of its operation
 * - an address register at IOAPICBASE+0 
 * - a data register at IOAPICBASE+0x10.
 * All accesses must be done on 4 byte boundaries.
 */

static void write_reg(const u8 sel_apic, const u8 offset, const u32 val)
{
	/* tell IOREGSEL where we want to write to */
	*IOSEL(sel_apic) = offset;
	/* write the value to IOWIN */
	*IOWIN(sel_apic) = val;
}

static u32 read_reg(const u8 sel_apic, const u8 offset)
{
	/* tell IOREGSEL where we want to read from */
	*IOSEL(sel_apic) = offset;
	/* read the value from IOWIN */
	return *IOWIN(sel_apic);
}

static u8 get_version(const u8 sel_apic)
{
	return read_reg(sel_apic, IOAPIC_VER_REG) & 0xFF;
}

static u8 get_apic_id(const u8 sel_apic)
{
	return (read_reg(sel_apic, IOAPIC_ID_REG) << 24) & 0xF;
}

void redirect_gsi(const u8 gsi, const u8 vector)
{
	for (int i = 0; i < io_apic_amount; i++) {
		struct madt_io_apic_data *cur = io_apic_list + i;
		if (gsi >= cur->gsi_base &&
		    gsi < cur->gsi_base + MAX_HANDLED_GSI) {
			u8 redir_entry = gsi - cur->gsi_base;
			write_reg(
				i, IOAPIC_RED_TBL_REGA(redir_entry),
				vector | // Interrupt vector
					(0 << 8) | // Delivery mode (0 = Fixed)
					(0
					 << 11) | // Destination mode (0 = Physical)
					(0 << 13) | // Polarity (1 = Active low)
					(1 << 15)); // Trigger (1 = Edge-triggered)
			write_reg(i, IOAPIC_RED_TBL_REGB(redir_entry),
				  0); // Send to APIC 0
			return;
		}
	}
	panic("GSI %d not found in any IOAPIC!\n", gsi);
}

void io_apic_setup(struct madt *madt)
{
	struct madt_entry *io_apic_entry = madt_find_entry(madt, IO_APIC);
  if (io_apic_entry == NULL)
    panic("No IO APIC found in MADT!\n");
	do {
		if (io_apic_amount == MAX_IO_APIC_HANDLED) {
			panic("Maximum IO APIC device reached");
		}
		struct madt_io_apic_data *io_apic_data =
			(void *)io_apic_entry->data;
		io_apic_list[io_apic_amount++] = *io_apic_data;
	} while ((io_apic_entry =
			  madt_find_next_entry(madt, io_apic_entry, IO_APIC)));

	redirect_gsi(KEYBOARD_GSI, IRQ_KEYBOARD);
}
