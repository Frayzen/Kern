#include "apic.h"
#include "drivers/apic/config.h"
#include "drivers/apic/madt.h"
#include "drivers/cpu_features.h"
#include "drivers/msr/msr.h"
#include "assert.h"
#include "drivers/rsdp/rsdp.h"
#include "interrupts/handler.h"
#include "panic.h"
#include "drivers/pic/pic.h"
#include <stdio.h>

#define IA32_APIC_BASE_MSR 0x1b
#define APIC_SW_ENABLE 0x100

static u32 volatile *lapic = 0; // only 32 bit aligned entries

// Although each register is 4 bytes, they are all aligned on a 16 byte boundary
#define APIC_REG(Offset) (lapic + Offset / 4)

// registers
#define LAPIC_ID_REG APIC_REG(0x20)
#define LAPIC_VERSION_REG APIC_REG(0x30)
#define LAPIC_EOI_REG APIC_REG(0xB0)
#define LAPIC_SIV_REG APIC_REG(0xF0) // Spurious Interrupt Vector Register
#define LAPIC_ERR_STATUS_REG APIC_REG(0x280)
#define LAPIC_TIMER_DIV_REG APIC_REG(0x3E0)
#define LAPIC_TIMER_INIT_COUNT APIC_REG(0x380)
#define LAPIC_TIMER_CUR_COUNT APIC_REG(0x390)
// local vector table registers
#define LAPIC_LVT_TIMER_REG APIC_REG(0x320)

#define IOAPIC_REDTBL_BASE 0x10

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define APIC_BASE_MSR_ENABLE (1 << 11)

/* Set the physical address for local APIC registers */
void set_apic_base(u32 apic)
{
	u32 edx = 0;
	u32 eax = (apic & 0xffffff000) | APIC_BASE_MSR_ENABLE;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
	edx = (apic >> 32) & 0x0f;
#endif

	msr_writew(IA32_APIC_BASE_MSR, eax, edx);

	u64 lapic_addr = apic;
	lapic = (u32 *)lapic_addr;

	assert((msr_readl(IA32_APIC_BASE_MSR) & 0xfffff0000) == lapic_addr);
}

/**
 * Get the physical address of the APIC registers page
 * make sure you map it to virtual memory ;)
 */
u32 cpu_get_apic_base()
{
	u32 eax;
	u64 res = msr_readl(IA32_APIC_BASE_MSR);
	eax = res >> 32;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
	u32 edx = res & 0xFFFFFFFF;
	return (eax & 0xfffff000) | ((edx & 0x0f) << 32);
#else
	return (eax & 0xfffff000);
#endif
}

void apic_send_eoi(void)
{
	*LAPIC_EOI_REG = 0;
}

void apic_setup(void)
{
	if (!check_cpuid_edx(CPUID_FEAT_EDX_APIC)) {
		panic("Cannot handle apic !");
	}
	pic_disable();

	struct MADT *madt = FIND_MADT;

	/* Hardware enable the Local APIC if it wasn't enabled */
	set_apic_base(madt->lapic_addr);
	assert(lapic == (u32 *)madt->lapic_addr);

	io_apic_setup(madt);

	// Set spurious vector and software enable apic
	*LAPIC_SIV_REG = APIC_SW_ENABLE | 0x40;

	// Setup APIC timer
	*LAPIC_LVT_TIMER_REG = LAPIC_LVT_TIMER_PERIODIC | 0x40;
	*LAPIC_LVT_TIMER_REG &= ~LAPIC_LVT_MASKED;

	*LAPIC_TIMER_DIV_REG = 3;
	*LAPIC_TIMER_INIT_COUNT = 0x8ffff;

	return;
}
