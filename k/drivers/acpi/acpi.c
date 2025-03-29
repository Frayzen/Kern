#include "acpi.h"
#include "drivers/cpu_features.h"
#include "drivers/msr/msr.h"
#include "drivers/rsdp/rsdp.h"
#include "drivers/rsdp/sdt.h"
#include "panic.h"
#include "drivers/pic/pic.h"
#include <cpuid.h>

#define IA32_APIC_BASE_MSR 0x1b

#define APIC_BASE ((u32 *)0xfee000000)

// registers
#define APIC_ID_REG (APIC_BASE + 0x20)
#define APIC_SIV_REG (APIC_BASE + 0x20) // Spurious Interrupt Vector Register

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define APIC_BASE_MSR_ENABLE 0x800

static int check_acpi(void)
{
	unsigned int eax, unused, edx = 0;
	__get_cpuid(1, &eax, &unused, &unused, &edx);
	return edx & CPUID_FEAT_EDX_APIC;
}

/* Set the physical address for local APIC registers */
void cpu_set_acpi_base(u32 acpi)
{
	u32 edx = 0;
	u32 eax = (acpi & 0xfffff0000) | APIC_BASE_MSR_ENABLE;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
	edx = (acpi >> 32) & 0x0f;
#endif

	msr_writew(IA32_APIC_BASE_MSR, eax, edx);
}

/**
 * Get the physical address of the APIC registers page
 * make sure you map it to virtual memory ;)
 */
u32 cpu_get_acpi_base()
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

void acpi_setup(void)
{
	if (!check_acpi()) {
		panic("Cannot handle acpi !");
	}
	struct MADT *madt = FIND_MADT;
	printf("Got apic addr %x\n", madt->apic_addr);
	pic_disable();

	/* Hardware enable the Local APIC if it wasn't enabled */
	cpu_set_acpi_base(cpu_get_acpi_base());

	/* Set the Spurious Interrupt Vector Register bit 8 to start receiving interrupts */
	/* *APIC_SIV_REG |= 0x100; */

	/* u64 base = msr_readl(IA32_APIC_BASE_MSR); */
	/* struct IA32_APIC_base *base_edit = (struct IA32_APIC_base *)&base; */
	/* base_edit->enable = 1; */
	/* msr_writel(IA32_APIC_BASE_MSR, base); */
	/* u64 base_address = base_edit->base << 12; */

	return;
}
