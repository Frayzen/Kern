#include "msi.h"
#include "drivers/pci/pci.h"
#include "assert.h"

void enable_msi(struct pci_device *device)
{
	u8 offset = device->capabilities.msi_cap_offset;
	assert(offset != 0);
	u32 id = pcidev_readw(device, offset + MSI_MID);
	struct MSI_identifier id_edit = *((struct MSI_identifier *)&id);
	assert(id_edit.capID == SMI_CAP_ID || id_edit.capID == SMIX_CAP_ID);

	u32 msg_ctl = pcidev_readw(device, offset + MSI_MC);
	struct MSI_msg_ctl msg_ctl_edit = *((struct MSI_msg_ctl *)&id);
	msg_ctl_edit.enable = 1;
	pcidev_writew(device, offset + MSI_MC, msg_ctl);

	printf("Enabled MSI for device\n");
	// FIXME finish the enabling of the msi
}

void disable_msi(struct pci_device *device)
{
	u8 offset = device->capabilities.msi_cap_offset;
	assert(offset != 0);
	u32 id = pcidev_readw(device, offset + MSI_MID);
	struct MSI_identifier id_edit = *((struct MSI_identifier *)&id);
	assert(id_edit.capID == SMI_CAP_ID || id_edit.capID == SMIX_CAP_ID);

	u32 msg_ctl = pcidev_readw(device, offset + MSI_MC);
	struct MSI_msg_ctl msg_ctl_edit = *((struct MSI_msg_ctl *)&id);
	msg_ctl_edit.enable = 0;
	pcidev_writew(device, offset + MSI_MC, msg_ctl);

	printf("Disabled MSI for device\n");
}
