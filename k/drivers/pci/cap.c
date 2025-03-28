#include "cap.h"
#include "drivers/pci/pci.h"
#include "assert.h"
#include <stdio.h>

void recurse_check(struct pci_device *device, u8 offset)
{
	u32 cap = pci_config_read(device->bus, device->slot, 0, offset);
	u8 capId = cap & 0xFF;
	u8 nextOffset = (cap >> 4) & 0xFF;

	if (capId == SMI_CAP_ID)
		device->capabilities.msi_cap_offset = offset;
	if (capId == SMIX_CAP_ID)
		device->capabilities.msix_cap_offset = offset;

	if (nextOffset == 0)
		return; // no more capabilities
	recurse_check(device, nextOffset);
}

void enable_msi(struct pci_device *device)
{
	u8 offset = device->capabilities.msi_cap_offset;
	assert(offset != 0);
	u32 msi_cap = pci_config_read(device->bus, device->slot, 0, offset);
	struct MSI_capability cap = *((struct MSI_capability *)&msi_cap);
	assert(cap.capID == SMI_CAP_ID || cap.capID == SMIX_CAP_ID);
	cap.enable = 1;
	pci_config_write(device->bus, device->slot, 0, offset, msi_cap);
	printf("Enabled MSI for device\n");
	// FIXME finish the enabling of the msi
}

void enable_msix(struct pci_device *device)
{
	u8 offset = device->capabilities.msix_cap_offset;
	assert(offset != 0);
	u32 msi_reg_1 = pci_config_read(device->bus, device->slot, 0, offset);
	struct MSIX_capability cap = *((struct MSIX_capability *)&msi_reg_1);
	assert(cap.capID == SMI_CAP_ID || cap.capID == SMIX_CAP_ID);
	cap.enable = 1;
	pci_config_write(device->bus, device->slot, 0, offset, msi_reg_1);

	const u32 msi_reg_2 =
		pci_config_read(device->bus, device->slot, 0, offset + 0x4);
	u32 table_offset = msi_reg_2 & (~0x7);
	u8 bir = msi_reg_2 & 0x7;
	u8 bar = PCI_BAR0 + 8 * bir;
	printf("BAR IS 0x%x\n", bar);

	volatile u32 bar_val = (volatile u32)get_bar(device, bar);
	volatile struct MSIX_vector_table *table_ptr =
		(volatile struct MSIX_vector_table *)(bar_val + table_offset);
#define APIC_BASE_MSI_ADDRESS 0xFEE00000

	table_ptr[0].masked = 0;
	table_ptr[0].msg_addr = APIC_BASE_MSI_ADDRESS & ~0x1;
	table_ptr[0].msg_data = 74;

	table_ptr[1].masked = 0;
	table_ptr[1].msg_addr = APIC_BASE_MSI_ADDRESS & ~0x1;
	table_ptr[1].msg_data = 74;

	printf("Found %d table size\n", cap.table_size);
	printf("Masked : %x\n", table_ptr[0].masked);
	printf("Data is %x\n", table_ptr[0].msg_data);
	printf("Addr is %x\n", table_ptr[0].msg_addr);
	pci_config_write(device->bus, device->slot, 0, offset, msi_reg_1);
	printf("Enabled MSIX for device\n");
}

void check_capacities(struct pci_device *device)
{
	if (device->status & PCI_STATUS_CAPABILITY_LIST_FLAG) {
		// check MSI capability
		u8 cap_offset = (pci_config_read(device->bus, device->slot, 0,
						 CAP_REG) &
				 0xFF);
		recurse_check(device, cap_offset);
	}
}
