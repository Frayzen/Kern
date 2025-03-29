#include "msix.h"
#include "assert.h"
#include "drivers/pci/pci.h"
#include "panic.h"
#include <stdio.h>

void enable_msix(struct pci_device *dev)
{
	u8 offset = dev->capabilities.msix_cap_offset;
	assert(offset != 0);

	u32 id = pcidev_readl(dev, offset + MSIX_MXID);
	struct MSIX_identifier *id_edit = ((struct MSIX_identifier *)&id);
	assert(id_edit->capID == SMIX_CAP_ID);

	id_edit->enable = 0;
	id_edit->function_mask = 1;
	pcidev_writel(dev, offset + MSIX_MXID, id);
	printf("Table size is %d\n", id_edit->table_size - 1);

	const u32 mtab = pcidev_readl(dev, offset + MSIX_MTAB);
	u32 table_offset = mtab & (~0x7);
	u8 bir = mtab & 0x7;
	u8 bar = PCI_BAR0 + 8 * bir;
	printf("MSIX used bar IS 0x%x\n", bar);

	u32 low_bar = get_bar(dev, bar) & 0xFFFFFFF0;
	u64 high_bar = get_bar(dev, bar + 0x4);
	u64 bar_val = (high_bar << 32) + low_bar;

	printf("Bar val is : 0x%x\n", low_bar);
	printf("Table offset is : 0x%x\n", table_offset);
	volatile struct MSIX_vector_table *table_ptr =
		(volatile struct MSIX_vector_table *)(bar_val + table_offset);
	printf("Table ptr is : 0x%x\n", table_ptr);
#define APIC_BASE_MSI_ADDRESS 0xFEE00000

	struct MSIX_vector_table ref_table = {
		ref_table.masked = 0,
		ref_table.msg_addr = APIC_BASE_MSI_ADDRESS & ~0xF,
		ref_table.msg_data = 0x41
	};

	id_edit->enable = 1;
	id_edit->function_mask = 0;
	pcidev_writel(dev, offset + MSIX_MXID, id);
	printf("Enabled MSIX for device\n");

	table_ptr[0] = ref_table;
	table_ptr[1] = ref_table;
	table_ptr[2] = ref_table;
}

void disable_msix(struct pci_device *dev)
{
	panic("NOT IMPLEMENTED");
}
