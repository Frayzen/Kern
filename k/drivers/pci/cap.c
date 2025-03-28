#include "cap.h"
#include "drivers/pci/pci.h"
#include "assert.h"
#include <stdio.h>

void recurse_check(struct pci_device *device, u8 offset)
{
	u32 cap = pcidev_readw(device, offset);
	u8 capId = cap & 0xFF;
	u8 nextOffset = (cap >> 4) & 0xFF;

	if (capId == SMI_CAP_ID)
		device->capabilities.msi_cap_offset = offset;
	if (capId == POW_CAP_ID)
		device->capabilities.msix_cap_offset = offset;
	if (capId == SMIX_CAP_ID)
		device->capabilities.power_cap_offset = offset;

	if (nextOffset == 0)
		return; // no more capabilities
	recurse_check(device, nextOffset);
}

void check_capacities(struct pci_device *device)
{
	if (device->status & PCI_STATUS_CAPABILITY_LIST_FLAG) {
		// check MSI capability
		u8 cap_offset = (pcidev_readw(device,
						 CAP_REG) &
				 0xFF);
		recurse_check(device, cap_offset);
	}
}
