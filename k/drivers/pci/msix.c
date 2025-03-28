#include "msix.h"
#include "assert.h"
#include "drivers/pci/msi.h"
#include "drivers/pci/pci.h"

void enable_msix(struct pci_device *dev)
{
	assert(dev->capabilities.msi_cap_offset &&
	       dev->capabilities.msix_cap_offset);
	disable_msi(dev);

	u8 offset = dev->capabilities.msix_cap_offset;
	assert(offset != 0);

	const u32 id = pcidev_readw(dev, offset + MSIX_MXID);
	struct MSIX_identifier id_edit = *((struct MSIX_identifier *)&id);
	assert(id_edit.capID == SMIX_CAP_ID);

	u32 msg_ctl = pcidev_readw(dev, offset + MSIX_MXC);
	struct MSIX_msg_ctl msg_ctl_edit = *((struct MSIX_msg_ctl *)&id);
	msg_ctl_edit.enable = 1;
	msg_ctl_edit.function_mask = 0;
	pcidev_writew(dev, offset + MSIX_MXC, id);

	const u32 mtab = pcidev_readl(dev, offset + MSIX_MTAB);
	u32 table_offset = mtab & (~0x7);
	u8 bir = mtab & 0x7;
	u8 bar = PCI_BAR0 + 8 * bir;
	printf("BAR IS 0x%x\n", bar);

	volatile u32 bar_val = (volatile u32)get_bar(dev, bar);
	volatile struct MSIX_vector_table *table_ptr =
		(volatile struct MSIX_vector_table *)(bar_val + table_offset);
#define APIC_BASE_MSI_ADDRESS 0xFEE00000

	table_ptr[0].masked = 0;
	table_ptr[0].msg_addr = APIC_BASE_MSI_ADDRESS & ~0x1;
	table_ptr[0].msg_data = 74;

	table_ptr[1].masked = 0;
	table_ptr[1].msg_addr = APIC_BASE_MSI_ADDRESS & ~0x1;
	table_ptr[1].msg_data = 74;

	pcidev_writew(dev, offset + MSIX_MXC, msg_ctl);
	printf("Enabled MSIX for device\n");
}
